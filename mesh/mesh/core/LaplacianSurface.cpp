#include "LaplacianSurface.h"
#include "MeshRenderer.h"
#include <gl/glew.h>

namespace hj
{
  LaplacianSurface::LaplacianSurface(MeshRenderer* ren)
    : ren_(ren)
    , smoothLSOWeight(0)
    , smoothLSOAnchor(10)
    , wijAll(NULL)
    , b3(NULL)
    , xyz(NULL)
    , ctrlmark(NULL)
    , OrigMesh(NULL)
  {
    InitTaucsInterface();
    Lc = CreateMatrix(1, 1);
    eye = Matrix(3, 3);
    for (int i = 1; i <= 3; i++)
    for (int j = 1; j <= 3; j++)
      i == j ? eye(i, j) = 1 : eye(i, j) = 0;
  }

  LaplacianSurface::~LaplacianSurface(void)
  {
    DeinitTaucsInterface();
    DEL_ARRAY(wijAll);
    DEL_ARRAY(b3);
    DEL_ARRAY(xyz);
    DEL_ARRAY(ctrlmark);
    DEL_ARRAY(OrigMesh);
  }

  inline void LaplacianSurface::computeCotWij()
  {
    DEL_ARRAY(wijAll);
    int n = (int)ren_->GetMesh()->n_vertices();
    wijAll = new float[n * 10];
    memset(wijAll, 0, sizeof(float)*n * 10);
    TriMesh::VertexVertexIter vv_it;
    TriMesh::VertexIter v_it;
    TriMesh::Scalar wij;
    TriMesh::Point p, p1, p2, p3;
    int pid, p1id, p2id, p3id;
    int wijID = 0;
    for (v_it = ren_->GetMesh()->vertices_begin(); v_it != ren_->GetMesh()->vertices_end(); ++v_it)
    {
      for (vv_it = ren_->GetMesh()->vv_iter(v_it); vv_it; ++vv_it)
      {
        // find alpha and beta (LSO paper), then calculate cotangent weights
        //    P    -- P3
        //   |     \     /
        //   P1-- P2
        // alpha is P_P1_P2, beta is P_P3_P2
        p = ren_->GetMesh()->point(v_it.handle());
        pid = v_it.handle().idx();
        p2 = ren_->GetMesh()->point(vv_it);
        p2id = vv_it.handle().idx();
        --vv_it;
        p3 = ren_->GetMesh()->point(vv_it);
        p3id = vv_it.handle().idx();
        ++vv_it;
        ++vv_it;
        p1 = ren_->GetMesh()->point(vv_it);
        p1id = vv_it.handle().idx();
        --vv_it;
        // wij = 1/2 * (cot(alpha) + cot(beta)), for boundary edge, there is only one such edge
        wij = 0;
        if (!ren_->GetMesh()->is_boundary(ren_->GetMesh()->find_halfedge(ren_->GetMesh()->vertex_handle(pid), ren_->GetMesh()->vertex_handle(p2id)))
          && !ren_->GetMesh()->is_boundary(ren_->GetMesh()->find_halfedge(ren_->GetMesh()->vertex_handle(p2id), ren_->GetMesh()->vertex_handle(pid)))) // not a boundary edge
          wij = ((p - p1) | (p2 - p1)) / ((p - p1) % (p2 - p1)).length() + ((p - p3) | (p2 - p3)) / ((p - p3) % (p2 - p3)).length();
        else // boundary edge, only have one such angle
        {
          if (p1id == p3id) // two angles are the same, eg. corner of a square
            wij = ((p - p1) | (p2 - p1)) / ((p - p1) % (p2 - p1)).length();
          else // find the angle not on the boundary
          {
            if (!ren_->GetMesh()->is_boundary(ren_->GetMesh()->find_halfedge(ren_->GetMesh()->vertex_handle(pid), ren_->GetMesh()->vertex_handle(p1id)))
              && !ren_->GetMesh()->is_boundary(ren_->GetMesh()->find_halfedge(ren_->GetMesh()->vertex_handle(p1id), ren_->GetMesh()->vertex_handle(pid))))
              wij = ((p - p1) | (p2 - p1)) / ((p - p1) % (p2 - p1)).length();
            else
              wij = ((p - p3) | (p2 - p3)) / ((p - p3) % (p2 - p3)).length();
          }
        }
        wijAll[wijID] = wij / 2;
        wijID++;
      }
    }
  }

  void LaplacianSurface::LSOLuXB()
  {
    if (smoothLSOWeight == 1)
      computeCotWij();
    TriMesh::VertexVertexIter vv_it;
    TriMesh::VertexIter v_it;
    int n = (int)ren_->GetMesh()->n_vertices();
    int numAnchor = (int)ceil(n*smoothLSOAnchor / 100.0); // number of anchor points
    int step = n / numAnchor + 1; // evenly choose anchor points with step size
    int len = n + numAnchor;
    Lu = CreateMatrix(len, n); // the Lu matrix is (n+numAnchor) by n
    DEL_ARRAY(b3);;
    b3 = new taucsType[len * 3]; // b3 is 3 times (n+numAnchor)
    memset(b3, 0, len * 3 * sizeof(taucsType));
    int VerID = 0; // go through the mesh by vertex iterator, thus I need an index to keep track of how many vertices visited
    int MatID = 0; // keep track of how many anchor points added to the matrix
    TriMesh::Scalar weight; // sum of wij
    int wijID = 0; // index for wij
    for (v_it = ren_->GetMesh()->vertices_begin(); v_it != ren_->GetMesh()->vertices_end(); ++v_it)
    {
      VerID++;
      // top half of Lu, n by n
      int vid = v_it.handle().idx();
      SetMatrixEntry(Lu, vid, vid, 1.0);
      int degree = 0;
      for (vv_it = ren_->GetMesh()->vv_iter(v_it); vv_it; ++vv_it)
        degree++;
      weight = 0;
      for (vv_it = ren_->GetMesh()->vv_iter(v_it); vv_it; ++vv_it)
      {
        // for Lu matrix, it's the same for both LuX=0 and LuX=b
        SetMatrixEntry(Lu, vid, vv_it.handle().idx(), -1.0 / degree);
        // find b in Lu X = b case
        if (smoothLSOWeight == 1)
        {
          b3[vid] += -wijAll[wijID] * ren_->GetMesh()->point(vv_it.handle())[0];
          b3[len + vid] += -wijAll[wijID] * ren_->GetMesh()->point(vv_it.handle())[1];
          b3[2 * len + vid] += -wijAll[wijID] * ren_->GetMesh()->point(vv_it.handle())[2];
          weight += wijAll[wijID];
          wijID++;
        }
      }
      if (smoothLSOWeight == 1) // Lu X = b, calculate b
      {
        b3[vid] /= weight;
        b3[vid] += ren_->GetMesh()->point(v_it.handle())[0];
        b3[len + vid] /= weight;
        b3[len + vid] += ren_->GetMesh()->point(v_it.handle())[1];
        b3[2 * len + vid] /= weight;
        b3[2 * len + vid] += ren_->GetMesh()->point(v_it.handle())[2];
      }
      // bottom half of Lu, numAnchor by n; bottom half of b3	
      if (VerID%step == 0){ // evenly choose points on the object
        int vid = v_it.handle().idx();
        SetMatrixEntry(Lu, n + MatID, vid, 1.0);
        b3[n + MatID] = ren_->GetMesh()->point(v_it.handle())[0];
        b3[len + n + MatID] = ren_->GetMesh()->point(v_it.handle())[1];
        b3[2 * len + n + MatID] = ren_->GetMesh()->point(v_it.handle())[2];
        MatID++;
      }
    }
    DEL_ARRAY(xyz);;
    xyz = new taucsType[ren_->GetMesh()->n_vertices() * 3];
    SolveATA(Lu, b3, xyz, 3);
    TriMesh::Point pttmp;
    // update points in mesh
    for (v_it = ren_->GetMesh()->vertices_begin(); v_it != ren_->GetMesh()->vertices_end(); ++v_it)
    {
      ren_->GetMesh()->point(v_it.handle())[0] = (float)xyz[v_it.handle().idx()];
      ren_->GetMesh()->point(v_it.handle())[1] = (float)xyz[v_it.handle().idx() + n];
      ren_->GetMesh()->point(v_it.handle())[2] = (float)xyz[v_it.handle().idx() + n * 2];
    }
    ReleaseMatrix(Lu);
  }

  void LaplacianSurface::PreCompute()
  {
    computeCotWij();
    int n = (int)ren_->GetMesh()->n_vertices();
    // initialize vidmark, b3 and xyz
    DEL_ARRAY(b3);;
    b3 = new taucsType[n * 3];
    memset(b3, 0, n * 3 * sizeof(taucsType));
    DEL_ARRAY(xyz);;
    xyz = new taucsType[n * 3];
    DEL_ARRAY(ctrlmark);;
    ctrlmark = new int[n];
    memset(ctrlmark, 0, n*sizeof(int));
    // mesh copy
    DEL_ARRAY(OrigMesh);;
    OrigMesh = new taucsType[n * 3];
    memset(OrigMesh, 0, n * 3 * sizeof(taucsType));
    // control points
    for (unsigned int i = 0; i<ren_->GetControlPts().size(); i++)
      ctrlmark[ren_->GetControlPts()[i].idx()] = 1;
    // anchor points
    for (unsigned int i = 0; i<ren_->GetAnchorPts().size(); i++)
      ctrlmark[ren_->GetAnchorPts()[i].idx()] = 1;
    // pre-allocate for vector R
    eye(3, 3) = 1;
    R.clear();
    R.reserve(sizeof(eye)*n);
    for (int i = 0; i<n; i++)
      R.push_back(eye);
    ReleaseMatrix(Lc);
    Lc = CreateMatrix(n, n);
    TriMesh::VertexIter v_it;
    TriMesh::VertexVertexIter vv_it;
    TriMesh::Scalar weight;
    int vid;
    // L matrix, n by n, cotangent weights
    int wijID = 0;
    for (v_it = ren_->GetMesh()->vertices_begin(); v_it != ren_->GetMesh()->vertices_end(); ++v_it)
    {
      vid = v_it.handle().idx();
      OrigMesh[vid] = ren_->GetMesh()->point(v_it.handle())[0];
      OrigMesh[vid + n] = ren_->GetMesh()->point(v_it.handle())[1];
      OrigMesh[vid + 2 * n] = ren_->GetMesh()->point(v_it.handle())[2];
      weight = 0;
      if (ctrlmark[v_it.handle().idx()] == 0)
      {
        for (vv_it = ren_->GetMesh()->vv_iter(v_it); vv_it; ++vv_it)
        {
          weight += wijAll[wijID];
          SetMatrixEntry(Lc, vid, vv_it.handle().idx(), -wijAll[wijID]);
          wijID++;
        }
        SetMatrixEntry(Lc, vid, vid, weight);
      }
      else
      {
        for (vv_it = ren_->GetMesh()->vv_iter(v_it); vv_it; ++vv_it)
          wijID++;
        SetMatrixEntry(Lc, vid, vid, 1.0);
      }
    }
    FactorATA(Lc);
  }

  inline void LaplacianSurface::SVDRotation()
  {
    TriMesh::VertexVertexIter vv_it;
    TriMesh::VertexIter v_it;
    Matrix U, V;
    DiagonalMatrix D;
    Matrix S;
    Matrix P(1, 1);
    Matrix Q(1, 1);
    int n = (int)ren_->GetMesh()->n_vertices();
    int vid, vvid;
    int wijID = 0; // index for wij
    for (v_it = ren_->GetMesh()->vertices_begin(); v_it != ren_->GetMesh()->vertices_end(); ++v_it)
    {
      int degree = 0;
      for (vv_it = ren_->GetMesh()->vv_iter(v_it); vv_it; ++vv_it)
        degree++;
      P.ReSize(3, degree);
      Q.ReSize(3, degree);
      degree = 0;
      vid = v_it.handle().idx();
      for (vv_it = ren_->GetMesh()->vv_iter(v_it); vv_it; ++vv_it)
      {
        // eij = pi - pj, pi is v_it, pj is vv_it, including weights wij
        degree++;
        vvid = vv_it.handle().idx();
        P(1, degree) = (OrigMesh[vid] - OrigMesh[vvid]) * wijAll[wijID];
        P(2, degree) = (OrigMesh[vid + n] - OrigMesh[vvid + n]) * wijAll[wijID];
        P(3, degree) = (OrigMesh[vid + 2 * n] - OrigMesh[vvid + 2 * n]) * wijAll[wijID];
        Q(1, degree) = xyz[vid] - xyz[vvid];
        Q(2, degree) = xyz[vid + n] - xyz[vvid + n];
        Q(3, degree) = xyz[vid + 2 * n] - xyz[vvid + 2 * n];
        wijID++;
      }
      // Compute the 3 by 3 covariance matrix
      // actually S = (P * W * Q.t()); W is already considerred in the previous step (P=P*W)
      S = (P * Q.t());
      // Compute the singular value decomposition S = UDV.t
      SVD(S, D, U, V);  // X = U * D * V.t()		
      eye(3, 3) = (V*U.t()).Determinant();
      // V*U.t may be reflection (determinant = -1). in this case, we need to change the sign of 
      // column of U corresponding to the smallest singular value (3rd column)
      R[v_it.handle().idx()] = (V * eye * U.t()); //Ri = (V * eye * U.t());
    }
  }

  void LaplacianSurface::ARAPDeform(int ARAPIteration)
  {
    int n = (int)ren_->GetMesh()->n_vertices();
    TriMesh::VertexVertexIter vv_it;
    TriMesh::VertexIter v_it;
    int vid, vvid;
    ColumnVector pijMat(3);
    ColumnVector RijPijMat(3);
    for (int iter = 0; iter <= ARAPIteration; iter++)
    {
      int wijID = 0;
      // update vector b3 = wij/2 * (Ri+Rj) * (pi - pj), where pi and pj are coordinates of the original mesh
      for (v_it = ren_->GetMesh()->vertices_begin(); v_it != ren_->GetMesh()->vertices_end(); ++v_it)
      {
        vid = v_it.handle().idx();
        if (ctrlmark[vid] == 0) // not control points
        {
          b3[vid] = 0;
          b3[vid + n] = 0;
          b3[vid + 2 * n] = 0;
          for (vv_it = ren_->GetMesh()->vv_iter(v_it); vv_it; ++vv_it)
          {
            vvid = vv_it.handle().idx();
            pijMat(1) = OrigMesh[vid] - OrigMesh[vvid];
            pijMat(2) = OrigMesh[vid + n] - OrigMesh[vvid + n];
            pijMat(3) = OrigMesh[vid + 2 * n] - OrigMesh[vvid + 2 * n];
            RijPijMat = ((R[vid] + R[vvid]) * pijMat);
            double wijtmp = wijAll[wijID] / 2;
            b3[vid] += RijPijMat(1) * wijtmp;
            b3[vid + n] += RijPijMat(2) * wijtmp;
            b3[vid + 2 * n] += RijPijMat(3) * wijtmp;
            wijID++;
          }
        }
        else
        {
          for (vv_it = ren_->GetMesh()->vv_iter(v_it); vv_it; ++vv_it)
            wijID++;
          b3[vid] = ren_->GetMesh()->point(v_it.handle())[0];
          b3[vid + n] = ren_->GetMesh()->point(v_it.handle())[1];
          b3[vid + 2 * n] = ren_->GetMesh()->point(v_it.handle())[2];
        }
      }
      SolveATA(Lc, b3, xyz, 3);
      if (iter > 0) // if iter = 0, just means naive LSE (Ri is identity matrix)
        SVDRotation();
    }
    // update vertices' coordinates 
    for (v_it = ren_->GetMesh()->vertices_begin(); v_it != ren_->GetMesh()->vertices_end(); ++v_it)
    {
      ren_->GetMesh()->point(v_it.handle())[0] = (float)xyz[v_it.handle().idx()];
      ren_->GetMesh()->point(v_it.handle())[1] = (float)xyz[v_it.handle().idx() + n];
      ren_->GetMesh()->point(v_it.handle())[2] = (float)xyz[v_it.handle().idx() + n * 2];
    }
  }

  void LaplacianSurface::translationDeform(TriMesh::Point &translation)
  {
    for (unsigned int i = 0; i<ren_->GetControlPts().size(); i++)
      ren_->GetMesh()->point(ren_->GetControlPts()[i]) = ren_->GetMesh()->point(ren_->GetControlPts()[i]) + translation;
  }

  void LaplacianSurface::rotationDeform(double angle, TriMesh::Point centroid, TriMesh::Point dir)
  {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(centroid[0], centroid[1], centroid[2]);
    glRotated(angle, dir[0], dir[1], dir[2]);
    glTranslated(-centroid[0], -centroid[1], -centroid[2]);
    GLdouble m[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, m);
    glPopMatrix();
    Matrix transform(4, 4);
    for (int i = 0; i<4; i++)
    for (int j = 0; j<4; j++)
      transform(j + 1, i + 1) = m[i * 4 + j];
    ColumnVector pt(4);
    for (unsigned int i = 0; i<ren_->GetControlPts().size(); i++)
    {
      for (int j = 0; j<3; j++)
        pt(j + 1) = (ren_->GetMesh()->point(ren_->GetControlPts()[i]))[j];
      pt(4) = 1;
      pt = transform * pt;
      for (int j = 0; j<3; j++)
        (ren_->GetMesh()->point(ren_->GetControlPts()[i]))[j] = (float)pt(j + 1);
    }
  }
}
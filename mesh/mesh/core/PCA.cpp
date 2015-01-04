#include "PCA.h"
#include "common/glgeometry.h"

namespace hj
{
  PCA::PCA()
    : mesh_(NULL)
  {

  }

  PCA::~PCA()
  {

  }

  void PCA::SetMesh(TriMesh* mesh)
  {
    mesh_ = mesh;
  }

  void PCA::getPCACentroid(std::vector<TriMesh::VHandle> &allVRoi)
  {
    centroid_[0] = 0;
    centroid_[1] = 0;
    centroid_[2] = 0;
    for (unsigned int i = 0; i<allVRoi.size(); i++)
    {
      centroid_[0] += (mesh_->point(allVRoi[i]))[0];
      centroid_[1] += (mesh_->point(allVRoi[i]))[1];
      centroid_[2] += (mesh_->point(allVRoi[i]))[2];
    }
    centroid_ /= (float)allVRoi.size();
  }

  void PCA::getPCAAxis(std::vector<TriMesh::VHandle> &allVRoi)
  {
    getPCACentroid(allVRoi);
    axis_.clear();
    obb_.clear();
    Matrix X((int)allVRoi.size(), 3);
    for (unsigned int i = 0; i<allVRoi.size(); i++)
    {
      X(i + 1, 1) = (mesh_->point(allVRoi[i]))[0] - centroid_[0];
      X(i + 1, 2) = (mesh_->point(allVRoi[i]))[1] - centroid_[1];
      X(i + 1, 3) = (mesh_->point(allVRoi[i]))[2] - centroid_[2];
    }
    Matrix S; S << X.t() * X;
    Matrix U, V;
    DiagonalMatrix D;
    SVD(S, D, U, V);  // X = U * D * V.t()
    Point ptmp;
    for (int i = 0; i<3; i++)
    {
      ptmp[0] = (float)U(1, i + 1);
      ptmp[1] = (float)U(2, i + 1);
      ptmp[2] = (float)U(3, i + 1);
      ptmp.normalize();
      axis_.push_back(ptmp);
    }
  }

  void PCA::getPCAOBB(std::vector<TriMesh::VHandle> &allVRoi)
  {
    getPCAAxis(allVRoi);
    Matrix rotate(4, 4);
    Matrix transformation = getPCATransformation();
    double xmax = -DBL_MAX, ymax = -DBL_MAX, zmax = -DBL_MAX;
    double xmin = DBL_MAX, ymin = DBL_MAX, zmin = DBL_MAX;
    ColumnVector pt(4);
    // find unoriented bounding box in original coordinate system
    for (unsigned int i = 0; i<allVRoi.size(); i++)
    {
      for (int j = 0; j<3; j++)
        pt(j + 1) = (mesh_->point(allVRoi[i]))[j];
      pt(4) = 1;
      pt = transformation * pt;
      if (pt(1)>xmax)
        xmax = pt(1);
      if (pt(1)<xmin)
        xmin = pt(1);
      if (pt(2)>ymax)
        ymax = pt(2);
      if (pt(2)<ymin)
        ymin = pt(2);
      if (pt(3)>zmax)
        zmax = pt(3);
      if (pt(3)<zmin)
        zmin = pt(3);
    }
    // front face:
    // 1 -- 2
    //  |      |
    // 3 -- 4
    addOneOBBPoint(xmin, ymax, zmax, transformation, pt);
    addOneOBBPoint(xmax, ymax, zmax, transformation, pt);
    addOneOBBPoint(xmax, ymax, zmin, transformation, pt);
    addOneOBBPoint(xmin, ymax, zmin, transformation, pt);
    // back face:
    // 5 -- 6
    //  |     |
    // 7 -- 8
    addOneOBBPoint(xmin, ymin, zmax, transformation, pt);
    addOneOBBPoint(xmax, ymin, zmax, transformation, pt);
    addOneOBBPoint(xmax, ymin, zmin, transformation, pt);
    addOneOBBPoint(xmin, ymin, zmin, transformation, pt);
  }

  Matrix PCA::getPCATransformation()
  {
    Matrix rotate(4, 4);
    rotate = 0;
    // rotation matrix is defined as:
    // ux1 ux2 ux3 0
    // uy1 uy2 uy3 0
    // uz1 uz2 uz3 0
    // 0 0 0 1
    // Here u is the axis, this matrix will rotate axis to original coordinate system
    for (int i = 0; i<3; i++)
    for (int j = 0; j<3; j++)
      rotate(i + 1, j + 1) = (axis_[i])[j];
    rotate(4, 4) = 1;
    Matrix translate(4, 4);
    translate = 0;
    // translation matrix is defined as:
    // 1 0 0 tx
    // 0 1 0 ty
    // 0 0 1 tz
    // 0 0 0 1
    // Here t is centroid, this matrix will translate centroid to original point (0,0,0)
    for (int i = 0; i<3; i++)
    {
      translate(i + 1, i + 1) = 1;
      translate(i + 1, 4) = centroid_[i];
    }
    translate(4, 4) = 1;
    Matrix transformation;
    transformation << rotate * translate;
    return transformation;
  }

  void PCA::addOneOBBPoint(double x, double y, double z, Matrix &transformation, ColumnVector & pt)
  {
    pt(1) = x; pt(2) = y; pt(3) = z;
    // transform vertices back, now they'll be vertices on OBB
    pt = transformation.i() * pt;
    Point ptobb;
    ptobb[0] = (float)pt(1); ptobb[1] = (float)pt(2); ptobb[2] = (float)pt(3);
    obb_.push_back(ptobb);
  }

  void PCA::drawPCAOBB()
  {
    glColor4d(0, 0.5, 0.5, 1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_DST_ALPHA, GL_DST_ALPHA);
    glDisable(GL_DEPTH_TEST);

    glBegin(GL_QUADS);
    // front
    glVertex3d((obb_[0])[0], (obb_[0])[1], (obb_[0])[2]);
    glVertex3d((obb_[1])[0], (obb_[1])[1], (obb_[1])[2]);
    glVertex3d((obb_[2])[0], (obb_[2])[1], (obb_[2])[2]);
    glVertex3d((obb_[3])[0], (obb_[3])[1], (obb_[3])[2]);
    // back
    glVertex3d((obb_[7])[0], (obb_[7])[1], (obb_[7])[2]);
    glVertex3d((obb_[6])[0], (obb_[6])[1], (obb_[6])[2]);
    glVertex3d((obb_[5])[0], (obb_[5])[1], (obb_[5])[2]);
    glVertex3d((obb_[4])[0], (obb_[4])[1], (obb_[4])[2]);
    // top
    glVertex3d((obb_[0])[0], (obb_[0])[1], (obb_[0])[2]);
    glVertex3d((obb_[4])[0], (obb_[4])[1], (obb_[4])[2]);
    glVertex3d((obb_[5])[0], (obb_[5])[1], (obb_[5])[2]);
    glVertex3d((obb_[1])[0], (obb_[1])[1], (obb_[1])[2]);
    // bottom
    glVertex3d((obb_[7])[0], (obb_[7])[1], (obb_[7])[2]);
    glVertex3d((obb_[3])[0], (obb_[3])[1], (obb_[3])[2]);
    glVertex3d((obb_[2])[0], (obb_[2])[1], (obb_[2])[2]);
    glVertex3d((obb_[6])[0], (obb_[6])[1], (obb_[6])[2]);
    // left
    glVertex3d((obb_[0])[0], (obb_[0])[1], (obb_[0])[2]);
    glVertex3d((obb_[3])[0], (obb_[3])[1], (obb_[3])[2]);
    glVertex3d((obb_[7])[0], (obb_[7])[1], (obb_[7])[2]);
    glVertex3d((obb_[4])[0], (obb_[4])[1], (obb_[4])[2]);
    // right
    glVertex3d((obb_[1])[0], (obb_[1])[1], (obb_[1])[2]);
    glVertex3d((obb_[5])[0], (obb_[5])[1], (obb_[5])[2]);
    glVertex3d((obb_[6])[0], (obb_[6])[1], (obb_[6])[2]);
    glVertex3d((obb_[2])[0], (obb_[2])[1], (obb_[2])[2]);
    glEnd();
    glBegin(GL_LINE_LOOP);
    // front
    glVertex3d((obb_[0])[0], (obb_[0])[1], (obb_[0])[2]);
    glVertex3d((obb_[1])[0], (obb_[1])[1], (obb_[1])[2]);
    glVertex3d((obb_[2])[0], (obb_[2])[1], (obb_[2])[2]);
    glVertex3d((obb_[3])[0], (obb_[3])[1], (obb_[3])[2]);
    glEnd();

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    glColor3d(0.5, 0.5, 0);
    glLineWidth(3);
    glDisable(GL_LIGHTING);
    glBegin(GL_LINE_LOOP);
    // back
    glVertex3d((obb_[7])[0], (obb_[7])[1], (obb_[7])[2]);
    glVertex3d((obb_[6])[0], (obb_[6])[1], (obb_[6])[2]);
    glVertex3d((obb_[5])[0], (obb_[5])[1], (obb_[5])[2]);
    glVertex3d((obb_[4])[0], (obb_[4])[1], (obb_[4])[2]);
    glEnd();
    glBegin(GL_LINE_LOOP);
    // top
    glVertex3d((obb_[0])[0], (obb_[0])[1], (obb_[0])[2]);
    glVertex3d((obb_[4])[0], (obb_[4])[1], (obb_[4])[2]);
    glVertex3d((obb_[5])[0], (obb_[5])[1], (obb_[5])[2]);
    glVertex3d((obb_[1])[0], (obb_[1])[1], (obb_[1])[2]);
    glEnd();
    glBegin(GL_LINE_LOOP);
    // bottom
    glVertex3d((obb_[7])[0], (obb_[7])[1], (obb_[7])[2]);
    glVertex3d((obb_[3])[0], (obb_[3])[1], (obb_[3])[2]);
    glVertex3d((obb_[2])[0], (obb_[2])[1], (obb_[2])[2]);
    glVertex3d((obb_[6])[0], (obb_[6])[1], (obb_[6])[2]);
    glEnd();
    glBegin(GL_LINE_LOOP);
    // left
    glVertex3d((obb_[0])[0], (obb_[0])[1], (obb_[0])[2]);
    glVertex3d((obb_[3])[0], (obb_[3])[1], (obb_[3])[2]);
    glVertex3d((obb_[7])[0], (obb_[7])[1], (obb_[7])[2]);
    glVertex3d((obb_[4])[0], (obb_[4])[1], (obb_[4])[2]);
    glEnd();
    glBegin(GL_LINE_LOOP);
    // right
    glVertex3d((obb_[1])[0], (obb_[1])[1], (obb_[1])[2]);
    glVertex3d((obb_[5])[0], (obb_[5])[1], (obb_[5])[2]);
    glVertex3d((obb_[6])[0], (obb_[6])[1], (obb_[6])[2]);
    glVertex3d((obb_[2])[0], (obb_[2])[1], (obb_[2])[2]);
    glEnd();
    glLineWidth(1);
    glEnable(GL_LIGHTING);
  }


  void PCA::getControlSphere(std::vector<TriMesh::VHandle> &controlPts)
  {
    if (controlPts.size()>3)
    {
      getPCAOBB(controlPts);
      radius_ = (obb_[1] - obb_[5]).length() * 0.5f;
    }
    else if (controlPts.size() >= 1)
    {
      centroid_ = mesh_->point(controlPts[0]);
      mesh_->needBoundingBox();
      radius_ = mesh_->getSceneRadius() / 15;
    }
  }

  void PCA::drawControlSphere()
  {
    // draw control sphere
    glColor4d(0, 0.5, 0.5, 0.3);
    glEnable(GL_BLEND);
    glBlendFunc(GL_DST_ALPHA, GL_DST_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslated(centroid_[0], centroid_[1], centroid_[2]);
    glScaled(radius_, radius_, radius_);
    glPushName(1);
    drawSolidSphere(1, 20, 20);
    glPopName();
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    // draw axis
    if (axis_.size() == 3)
    {
      glLineWidth(3);
      glColor3d(1, 1, 1);
      glDisable(GL_LIGHTING);
      glScaled(1.5, 1.5, 1.5);
      glBegin(GL_LINES);
      glColor3d(1, 0, 0);
      glVertex3d(-axis_[0][0], -axis_[0][1], -axis_[0][2]);
      glVertex3d(axis_[0][0], axis_[0][1], axis_[0][2]);
      glColor3d(0, 1, 0);
      glVertex3d(-axis_[1][0], -axis_[1][1], -axis_[1][2]);
      glVertex3d(axis_[1][0], axis_[1][1], axis_[1][2]);
      glColor3d(0, 0, 1);
      glVertex3d(-axis_[2][0], -axis_[2][1], -axis_[2][2]);
      glVertex3d(axis_[2][0], axis_[2][1], axis_[2][2]);
      glEnd();
      glLineWidth(1);
      glEnable(GL_LIGHTING);
    }

    glPopMatrix();
  }

  Vec PCA::getCentroid()
  {
    Vec tmp;
    tmp[0] = centroid_[0];
    tmp[1] = centroid_[1];
    tmp[2] = centroid_[2];
    return tmp;
  }
}
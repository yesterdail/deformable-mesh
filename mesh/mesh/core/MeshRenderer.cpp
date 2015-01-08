#include "MeshRenderer.h"
#include "common/GLFramebuffer.h"
#include "common/GLTexture.h"
#include "common/Trackball.h"
#include "common/ImageIO.h"
#include "common/Image.h"
#include "common/ScanLine.h"
#include "common/glgeometry.h"
#include "PCA.h"
#include "LaplacianSurface.h"
#include "roi/GraphicsRenderer.h"
#include "roi/GraphicsLine.h"

namespace hj
{
  MeshRenderer::MeshRenderer()
    : out_fbo_ptr_(NULL)
    , mesh_(NULL)
    , texture_image_(NULL)
    , pcaAnchor_(NULL)
    , pcaControl_(NULL)
    , ls_(NULL)
    , depth_buffer_(NULL)
    , center_(Point(0, 0, 0))
    , radius_(1)
    , fovy_(45)
    , wireframe_(true)
    , solid_(true)
    , texture_(false)
    , isPreComputed_(false)
    , meshfile_("")
    , depth_(0.9f)
    , gren_(NULL)
  {
    glDisable(GL_DITHER);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    DEL_PTR(pcaAnchor_);
    pcaAnchor_ = new PCA();
    DEL_PTR(pcaControl_);
    pcaControl_ = new PCA();
    DEL_PTR(ls_);
    ls_ = new LaplacianSurface(this);
  }

  MeshRenderer::~MeshRenderer()
  {
    DEL_PTR(mesh_);
    DEL_PTR(texture_image_);
    DEL_PTR(pcaAnchor_);
    DEL_PTR(pcaControl_);
    DEL_PTR(ls_);
    DEL_ARRAY(depth_buffer_);
  }

  bool MeshRenderer::SetFBO(GLFramebuffer* fbo)
  {
    out_fbo_ptr_ = fbo;

    DEL_ARRAY(depth_buffer_);
    depth_buffer_ = new float[out_fbo_ptr_->GetWidth() * out_fbo_ptr_->GetHeight()];
    return true;
  }

  void MeshRenderer::SetAssociate(GraphicsRenderer* gren)
  {
    gren_ = gren;
  }

  bool MeshRenderer::Run()
  {
    out_fbo_ptr_->Bind();
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    glClearColor(0, 0, 0, 0);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float fmodelview[16], fprojection[16];
    glm::get(camera_.GetViewMatrix() * model_, fmodelview);
    glm::get(camera_.GetProjectionMatrix(), fprojection);

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(fmodelview);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(fprojection);

    //drawSolidCylinder(0.2, 1, 20, 1);
    //drawSolidCylinder(0.25, 1, 20, 20);
    //drawSolidCylinder(0.2, 1, 20, 20);

    // TODO: 
    if (mesh_)
    {
      glEnable(GL_DEPTH_TEST);
      glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(3, GL_FLOAT, 0, mesh_->points());
      glEnableClientState(GL_NORMAL_ARRAY);
      glNormalPointer(GL_FLOAT, 0, mesh_->vertex_normals());

      // texture
      if (texture_ && texture_image_ && mesh_->has_vertex_texcoords2D())
      {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, mesh_->texcoords2D());
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture_image_->GetTexture()->GetId());
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      }

      // draw solid mesh, with polygon offset
      if (solid_)
      {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(2.5f, 2.5f);
        drawROI(0.8, 0, 0);
        drawMainObject(0.8f, 1.0f, 1.0f);
      }
      if (wireframe_)
      {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        drawROI(0.8, 0, 0);
        drawMainObject(0.5f, 0.5f, 0.5f);
      }

      // draw control and anchor points.
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      drawAnchorAndControl();

      glDisableClientState(GL_VERTEX_ARRAY);
      glDisableClientState(GL_NORMAL_ARRAY);
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      glDisable(GL_TEXTURE_2D);
      glDisable(GL_DEPTH_TEST);
    }
    out_fbo_ptr_->Unbind();
    return (GL_NO_ERROR == glGetError());
  }

  bool MeshRenderer::LoadMesh(const std::string& filename)
  {
    DEL_PTR(mesh_);
    mesh_ = new TriMesh();
    if (!mesh_->read(filename.c_str())) {
      assert(0);
      return false;
    }

    mesh_->needBoundingBox();
    center_ = mesh_->getSceneCenter();
    radius_ = (float)mesh_->getSceneRadius();

    TriMesh::ConstFaceIter fit(mesh_->faces_begin()), fEnd(mesh_->faces_end());
    triverts_.clear();
    for (; fit != fEnd; ++fit)
    {
      TriMesh::ConstFaceVertexIter fvit = mesh_->cfv_iter(fit.handle());
      triverts_.push_back(fvit.handle().idx()); ++fvit;
      triverts_.push_back(fvit.handle().idx()); ++fvit;
      triverts_.push_back(fvit.handle().idx());
    }

    ResetCamera();

    pcaAnchor_->SetMesh(mesh_);
    pcaControl_->SetMesh(mesh_);

    meshfile_ = filename;
    return true;
  }

  bool MeshRenderer::LoadTexture(const std::string& filename)
  {
    DEL_PTR(texture_image_);
    texture_image_ = new Image();
    if (!ImageIO::Read(filename, texture_image_)) {
      DEL_PTR(texture_image_);
      assert(0);
      return false;
    }
    texture_image_->CreateTexture();
    if (glm::any(glm::lessThanEqual(texture_image_->GetSize(), glm::ivec2(0)))) {
      assert(0);
      return false;
    }

    return true;
  }

  bool MeshRenderer::ResetCamera()
  {
    int width = out_fbo_ptr_->GetWidth();
    int height = out_fbo_ptr_->GetHeight();

    model_ = glm::translate(glm::vec3(-center_[0], -center_[1], -center_[2]));

    glm::vec3 eye = glm::vec3(0, 0, radius_ * 4);
    glm::vec3 lookat = glm::vec3(0, 0, 0);
    glm::vec3 updir = glm::vec3(0, 1, 0);
    float zNear = std::abs(eye.z) - radius_;
    float zFar = std::abs(eye.z) + radius_;
    float aspect = (float)width / height;
    float fovy = rad2deg(atan(radius_ / aspect / zNear)) * 2;
    camera_.Initialize(eye,
      lookat,
      updir,
      fovy,
      aspect,
      zNear,
      zFar);

    trackball_.SetCamera(&camera_);
    return true;
  }

  void MeshRenderer::Rotate(float newMouseX,
    float newMouseY,
    float lastMouseX,
    float lastMouseY)
  {
    int w = out_fbo_ptr_->GetWidth();
    int h = out_fbo_ptr_->GetHeight();

    trackball_.Rotate(glm::vec2(newMouseX, newMouseY),
      glm::vec2(lastMouseX, lastMouseY),
      w, h);
  }

  void MeshRenderer::Zoom(float newMouseX,
    float newMouseY,
    float lastMouseX,
    float lastMouseY)
  {
    int w = out_fbo_ptr_->GetWidth();
    int h = out_fbo_ptr_->GetHeight();
    trackball_.Zoom(glm::vec2(newMouseX, newMouseY),
      glm::vec2(lastMouseX, lastMouseY),
      w, h);
  }

  void MeshRenderer::Move(float newMouseX,
    float newMouseY,
    float lastMouseX,
    float lastMouseY)
  {
    int w = out_fbo_ptr_->GetWidth();
    int h = out_fbo_ptr_->GetHeight();

    trackball_.Move(glm::vec2(newMouseX, newMouseY),
      glm::vec2(lastMouseX, lastMouseY),
      w, h);
  }

  void MeshRenderer::drawMainObject(float r, float g, float b)
  {
    glColor3f(r, g, b);
    if (triverts_.size()>0)
      glDrawElements(GL_TRIANGLES, (GLsizei)triverts_.size(), GL_UNSIGNED_INT, &(triverts_[0]));
  }

  void MeshRenderer::drawAnchorAndControl()
  {
    if (anchorPts_.size()>0)
    {
      glColor3d(1, 0, 0);
      pcaAnchor_->drawPCAOBB();
    }
    if (controlPts_.size()>0)
    {
      glColor3d(0, 1, 0);
      pcaControl_->drawControlSphere();
    }
  }

  void MeshRenderer::drawROI(double r, double g, double b)
  {
    glColor3d(r, g, b);
    if (roiverts_.size() > 0)
      glDrawElements(GL_TRIANGLES, (GLsizei)roiverts_.size(), GL_UNSIGNED_INT, &(roiverts_[0]));
  }

  void MeshRenderer::SetSmooth()
  {
    glShadeModel(GL_SMOOTH);
  }

  void MeshRenderer::SetFlat()
  {
    glShadeModel(GL_FLAT);
  }

  void MeshRenderer::SetWireframe(bool w)
  {
    wireframe_ = w;
  }

  void MeshRenderer::SetSolid(bool s)
  {
    solid_ = s;
  }

  void MeshRenderer::SetTexture(bool t)
  {
    texture_ = t;
  }

  void MeshRenderer::getLasso2dRegion(const std::vector<glm::vec2> &polygon)
  {
    if (polygon.size() <= 2) return;

    double maxx = -DBL_MAX;
    double maxy = -DBL_MAX;
    double minx = DBL_MAX;
    double miny = DBL_MAX;
    for (size_t i = 0; i < polygon.size(); i++)
    {
      if (maxx< polygon[i].x) maxx = polygon[i].x;
      if (maxy< polygon[i].y) maxy = polygon[i].y;
      if (minx> polygon[i].x) minx = polygon[i].x;
      if (miny> polygon[i].y) miny = polygon[i].y;
    }

    //glm::mat4 volume2ClipCoord = projection_ * modelview_;
    glm::mat4 volume2ClipCoord = camera_.GetViewProjectionMatrix() * model_;
    int view_width = out_fbo_ptr_->GetWidth();
    int view_height = out_fbo_ptr_->GetHeight();

    std::vector<TriMesh::VHandle> vset;
    TriMesh::VertexIter v_it;
    for (v_it = mesh_->vertices_begin(); v_it != mesh_->vertices_end(); ++v_it)
    {
      TriMesh::Point p = mesh_->point(v_it.handle());

      // convert to view coordinate.
      glm::vec4 point(p[0], p[1], p[2], 1);
      point = volume2ClipCoord * point;
      // NDC
      point /= point.w;
      // Window Coordinate
      point.x = point.x * view_width * 0.5f + view_width * 0.5f;
      point.y = point.y * view_height * 0.5f + view_height * 0.5f;

      if (point[0] < minx || point[0] > maxx || point[1] < miny || point[1] > maxy)
        continue;
      if (ScanLine::PointInPolygon(polygon, glm::dvec2(point)))
        vset.push_back(v_it.handle());
    }
    vertex2Face(vset, curFRoi_);
    vset.clear();

    // boolean operations
    allFRoi_ = curFRoi_;

    roiverts_.clear();
    for (unsigned int i = 0; i<allFRoi_.size(); i++)
    {
      TriMesh::ConstFaceVertexIter fvit = mesh_->cfv_iter(allFRoi_[i]);
      roiverts_.push_back(fvit.handle().idx());	++fvit;
      roiverts_.push_back(fvit.handle().idx());	++fvit;
      roiverts_.push_back(fvit.handle().idx());
    }
  }

  void MeshRenderer::vertex2Face(std::vector<TriMesh::VHandle> &vset,
    std::vector<TriMesh::FHandle> &bfs)
  {
    bfs.clear();
    TriMesh::VertexFaceIter vf_it;
    for (unsigned int i = 0; i<vset.size(); i++)
    {
      for (vf_it = mesh_->vf_iter(vset[i]); vf_it; ++vf_it)
      {
        if (vf_it.handle().is_valid())
          bfs.push_back(vf_it.handle());
      }
    }
    std::vector<TriMesh::FHandle>::iterator it;
    std::sort(bfs.begin(), bfs.end());
    it = std::unique(bfs.begin(), bfs.end());
    bfs.resize(it - bfs.begin());
  }

  void MeshRenderer::face2Vertex(std::vector<TriMesh::VHandle> &vset,
    std::vector<TriMesh::FHandle> &fset)
  {
    TriMesh::FaceVertexIter fv_it;
    for (unsigned int i = 0; i<fset.size(); i++)
    {
      for (fv_it = mesh_->fv_iter(fset[i]); fv_it; ++fv_it)
      {
        if (fv_it.handle().is_valid())
          vset.push_back(fv_it.handle());
      }
    }
    std::vector<TriMesh::VHandle>::iterator it;
    std::sort(vset.begin(), vset.end());
    it = std::unique(vset.begin(), vset.end());
    vset.resize(it - vset.begin());
  }

  void MeshRenderer::SetAnchorPoints(const std::vector<glm::vec2> &polygon)
  {
    if (!mesh_) return;
    getLasso2dRegion(polygon); // get ROI

    allVRoi_.clear();
    face2Vertex(allVRoi_, allFRoi_);

    anchorPts_ = allVRoi_;
    pcaAnchor_->getPCAOBB(anchorPts_);

    roiverts_.clear();
    isPreComputed_ = false;
  }

  void MeshRenderer::SetControlPoints(const std::vector<glm::vec2> &polygon)
  {
    if (!mesh_) return;
    getLasso2dRegion(polygon); // get ROI

    allVRoi_.clear();
    face2Vertex(allVRoi_, allFRoi_);

    controlPts_ = allVRoi_;
    pcaControl_->getControlSphere(controlPts_);

    roiverts_.clear();
    isPreComputed_ = false;
  }

  bool MeshRenderer::PostSelection(const glm::vec2 &point)
  {
    Vec centroid_world = pcaControl_->getCentroid();
    Vec centroid_view = Model2View(centroid_world);
    double length = std::sqrt((centroid_view[0] - point[0]) * (centroid_view[0] - point[0]) +
      (centroid_view[1] - point[1]) * (centroid_view[1] - point[1]));

    if (length <= 30
      && controlPts_.size() > 0
      && anchorPts_.size() > 0) {
      // select control points for deformation
      curPointInWorld_ = pcaControl_->getCentroid();
      initialPointInCam_ = Model2View(curPointInWorld_);
      return true;
    }

    return false;
  }

  bool MeshRenderer::Deformation(const glm::vec2 &point)
  {
    if (!isPreComputed_) {
      ls_->PreCompute();
      isPreComputed_ = true;
    }
    movingPointInWorld_[0] = point.x;
    movingPointInWorld_[1] = point.y;
    movingPointInWorld_[2] = initialPointInCam_[2]; // correct depth, now data->movingPointInWorld is actually the mouse position in camera with correct depth
    movingPointInWorld_ = View2Model(movingPointInWorld_); // data->movingPointInWorld is projected to world coordinate system
    translationInWorld_[0] = (movingPointInWorld_ - curPointInWorld_)[0]; // translation displacement in world coordinate system
    translationInWorld_[1] = (movingPointInWorld_ - curPointInWorld_)[1];
    translationInWorld_[2] = (movingPointInWorld_ - curPointInWorld_)[2];
    curPointInWorld_ = movingPointInWorld_; // update 
    ls_->translationDeform(translationInWorld_); // deformation caused by translation
    ARAPIteration_<3 ? ls_->ARAPDeform(ARAPIteration_) : ls_->ARAPDeform(3); // intermediate result, naive LSE
    pcaControl_->getControlSphere(controlPts_);

    // update mesh center and radius.
    mesh_->needBoundingBox();
    center_ = mesh_->getSceneCenter();
    radius_ = (float)mesh_->getSceneRadius();
    return true;
  }

  Vec MeshRenderer::Model2View(const Vec &p)
  {
    double dmodelview[16], dprojection[16];
    glm::get(camera_.GetViewMatrix() * model_, dmodelview);
    glm::get(camera_.GetProjectionMatrix(), dprojection);
    GLint viewport[4] = { 0, 0, out_fbo_ptr_->GetWidth(), out_fbo_ptr_->GetHeight() };
    glGetIntegerv(GL_VIEWPORT, viewport);

    double x, y, z;
    gluProject(p[0], p[1], p[2], dmodelview, dprojection, viewport, &x, &y, &z);

    return Vec((float)x, (float)y, (float)z);
  }

  Vec MeshRenderer::View2Model(const Vec &p)
  {
    double dmodelview[16], dprojection[16];
    glm::get(camera_.GetViewMatrix() * model_, dmodelview);
    glm::get(camera_.GetProjectionMatrix(), dprojection);
    GLint viewport[4] = { 0, 0, out_fbo_ptr_->GetWidth(), out_fbo_ptr_->GetHeight() };

    double x, y, z;
    gluUnProject(p[0], p[1], p[2], dmodelview, dprojection, viewport, &x, &y, &z);

    return Vec((float)x, (float)y, (float)z);
  }

  void MeshRenderer::CancelDeform()
  {
    anchorPts_.clear();
    controlPts_.clear();
  }

  void MeshRenderer::RestoreMesh()
  {
    if (meshfile_.empty() || !mesh_) return;

    if (LoadMesh(meshfile_))
    {
      CancelDeform();
    }
  }

  void MeshRenderer::CutMesh()
  {
    if (!gren_ || gren_->GetGraphics().empty()) return;

    GraphicsBase* obj = gren_->GetGraphics()[0];
    if (obj->GetClassName() != "GraphicsLine") return;

    GraphicsLine* gline = (GraphicsLine*)obj;
    glm::vec2 start = gline->GetStart();
    glm::vec2 end = gline->GetEnd();

    if (!mesh_) return;
    curFRoi_.clear();
    roiverts_.clear();

    // get depth buffer
    int width = out_fbo_ptr_->GetWidth();
    int height = out_fbo_ptr_->GetHeight();
    out_fbo_ptr_->Bind();
    glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, depth_buffer_);
    out_fbo_ptr_->Unbind();

    TriMesh::FaceIter f_it;
    TriMesh::FaceVertexIter fv_it;
    TriMesh::Point p;
    std::vector<TriMesh::Point> triangle;
    for (f_it = mesh_->faces_begin(); f_it != mesh_->faces_end(); ++f_it)
    {
      // get face vertices.
      triangle.clear();
      for (fv_it = mesh_->fv_iter(f_it); fv_it.is_valid(); ++fv_it)
      {
        p = mesh_->point(fv_it);  // model
        p = Model2View(p);        // view
        triangle.push_back(p);
      }

      if (triangle.size() != 3) {
        assert(0);
        return;
      }

      // judge intersection with line.
      if (ScanLine::Intersecting(start, end,
        glm::vec2(triangle[0][0], triangle[0][1]),
        glm::vec2(triangle[1][0], triangle[1][1]),
        glm::vec2(triangle[2][0], triangle[2][1])))
      {
        // depth test
        //if (triangle[0][2] <= depth_ //depth_buffer_[(int)triangle[0][1] * width + (int)triangle[0][0]]
        //  || triangle[1][2] <= depth_ // depth_buffer_[(int)triangle[1][1] * width + (int)triangle[1][0]]
        //  || triangle[2][2] <= depth_) // depth_buffer_[(int)triangle[2][1] * width + (int)triangle[2][0]])
        if (triangle[0][2] <= depth_buffer_[(int)triangle[0][1] * width + (int)triangle[0][0]]
          || triangle[1][2] <= depth_buffer_[(int)triangle[1][1] * width + (int)triangle[1][0]]
          || triangle[2][2] <= depth_buffer_[(int)triangle[2][1] * width + (int)triangle[2][0]])
          curFRoi_.push_back(f_it);
      }
    }

    // face to vertices
    for (unsigned int i = 0; i<curFRoi_.size(); i++)
    {
      TriMesh::ConstFaceVertexIter fvit = mesh_->cfv_iter(curFRoi_[i]);
      roiverts_.push_back(fvit.handle().idx());	++fvit;
      roiverts_.push_back(fvit.handle().idx());	++fvit;
      roiverts_.push_back(fvit.handle().idx());
    }

    gren_->RemoveAll();
  }

  void MeshRenderer::SetLineDepth(float d)
  {
    if (std::abs(depth_ - d) < kEpsilonFloat) return;

    if (d < 0) d = 0;
    if (d > 1) d = 1;
    depth_ = d;
    CutMesh();
  }
}
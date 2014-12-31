#include "MeshRenderer.h"
#include "common/GLFramebuffer.h"
#include "common/GLTexture.h"
#include "common/Trackball.h"

namespace hj
{
  MeshRenderer::MeshRenderer()
    : out_fbo_ptr_(NULL)
    , mesh_(NULL)
    , center_(Point(0, 0, 0))
    , radius_(1)
    , fovy_(45)
  {
  }

  MeshRenderer::~MeshRenderer()
  {
    DEL_PTR(out_fbo_ptr_);
    DEL_PTR(mesh_);
  }

  bool MeshRenderer::Initialize(const glm::ivec2& view_size)
  {
    if (!ResizeOutput(view_size)) {
      return false;
    }

    glDisable(GL_DITHER);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    return true;
  }

  void MeshRenderer::GetPixel(uint8_t* pixels,
    uint32_t pixel_length,
    bool swap_channels)
  {
    if (!pixels) {
      assert(0);
      return;
    }
    if (!Run()) {
      assert(0);
      return;
    }
    GLTexture* texture_ptr = out_fbo_ptr_->GetTexture2D(GL_COLOR_ATTACHMENT0);
    texture_ptr->DownloadTexture(pixels, pixel_length);
    glFinish();
    if (swap_channels) {
      int pixel_count = pixel_length / 3;
      uint8_t* ptr = pixels;
      for (int i = 0; i < pixel_count; ++i) {
        uint8_t tmp = ptr[2];
        ptr[2] = ptr[0];
        ptr[0] = tmp;
        ptr += 3;
      }
    }
  }

  bool MeshRenderer::Run()
  {
    glClearColor(0, 0, 0, 0);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float fmodelview[16], fprojection[16];
    glm::get(modelview_, fmodelview);
    glm::get(projection_, fprojection);

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(fmodelview);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(fprojection);

    // TODO: 
    if (mesh_)
    {
      glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(3, GL_FLOAT, 0, mesh_->points());
      glEnableClientState(GL_NORMAL_ARRAY);
      glNormalPointer(GL_FLOAT, 0, mesh_->vertex_normals());

      // draw solid mesh, with polygon offset
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glEnable(GL_POLYGON_OFFSET_FILL);
      glPolygonOffset(2.5f, 2.5f);
      drawMainObject(0.8f, 1.0f, 1.0f);

      glDisableClientState(GL_VERTEX_ARRAY);
      glDisableClientState(GL_NORMAL_ARRAY);
    }

    return (GL_NO_ERROR == glGetError());
  }

  bool MeshRenderer::ResizeOutput(const glm::ivec2 &new_size)
  {
    DEL_PTR(out_fbo_ptr_);
    out_fbo_ptr_ = new GLFramebuffer(new_size.x, new_size.y);
    out_fbo_ptr_->CreateColorTexture(GL_COLOR_ATTACHMENT0,
      GL_RGB8,
      GL_RGB,
      GL_UNSIGNED_BYTE);
    out_fbo_ptr_->CreateDepthTexture();
    if (!out_fbo_ptr_->IsOk()) {
      assert(0);
      return false;
    }

    return true;
  }

  bool MeshRenderer::LoadMesh(const std::string& filename)
  {
    DEL_PTR(mesh_);
    mesh_ = TriMesh::read(filename.c_str());
    if (!mesh_) {
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

    return true;
  }

  bool MeshRenderer::ResetCamera()
  {
    int width = out_fbo_ptr_->GetWidth();
    int height = out_fbo_ptr_->GetHeight();

    modelview_ = glm::translate(glm::dvec3(
      -center_[0],
      -center_[1],
      -(center_[2] + radius_ * 2.0f)));

    projection_ = glm::perspective(
      fovy_,
      (float)width / height,
      0.01f * radius_,
      100.0f * radius_);

    return true;
  }

  void MeshRenderer::Rotate(float newMouseX,
    float newMouseY,
    float lastMouseX,
    float lastMouseY)
  {
    int w = out_fbo_ptr_->GetWidth();
    int h = out_fbo_ptr_->GetHeight();

    glm::mat4 rotate_matrix = TrackBall::RotateMatrix(
      glm::vec2(lastMouseX, lastMouseY),
      glm::vec2(newMouseX, newMouseY),
      glm::ivec2(w, h));

    glm::vec4 center_world = modelview_ *
      glm::vec4(center_[0], center_[1], center_[2], 1);

    modelview_ = glm::translate(glm::vec3(center_world)) *
      rotate_matrix *
      glm::translate(glm::vec3(-center_world)) *
      modelview_;
  }

  void MeshRenderer::Zoom(float /*newMouseX*/,
    float newMouseY,
    float /*lastMouseX*/,
    float lastMouseY)
  {
    int h = out_fbo_ptr_->GetHeight();
    float dy = lastMouseY - newMouseY;
    float value_y = radius_ * dy * 3.0f / h;

    modelview_ = glm::translate(glm::vec3(0.0, 0.0, value_y))
      * modelview_;
  }

  void MeshRenderer::Move(float newMouseX,
    float newMouseY,
    float lastMouseX,
    float lastMouseY)
  {
    int w = out_fbo_ptr_->GetWidth();
    int h = out_fbo_ptr_->GetHeight();

    newMouseY = h - newMouseY;
    lastMouseY = h - lastMouseY;

    float dx = newMouseX - lastMouseX;
    float dy = newMouseY - lastMouseY;

    glm::vec4 center_world = modelview_ *
      glm::vec4(center_[0], center_[1], center_[2], 1);
    float z = -center_world.z / center_world.w;

    float aspect = (float)w / h;
    float near_plane = 0.01f * radius_;
    float top = tan(fovy_ / 2.0f * kPI / 180.0f) * near_plane;
    float right = aspect*top;
    modelview_ = glm::translate(glm::vec3(
      2.0*dx / w*right / near_plane*z,
      -2.0*dy / h*top / near_plane*z,
      0.0f)) * modelview_;
  }

  void MeshRenderer::drawMainObject(float r, float g, float b)
  {
    glColor3f(r, g, b);
    if (triverts_.size()>0)
      glDrawElements(GL_TRIANGLES, (GLsizei)triverts_.size(), GL_UNSIGNED_INT, &(triverts_[0]));
  }
}
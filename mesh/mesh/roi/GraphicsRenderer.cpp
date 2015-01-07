#include "GraphicsRenderer.h"
#include "common/GLTexture.h"
#include "common/GLFramebuffer.h"
#include "GraphicsBase.h"
#include "ToolBase.h"
#include "ToolPointer.h"
#include "ToolLine.h"

namespace hj
{
  GraphicsRenderer::GraphicsRenderer()
    : out_fbo_ptr_(NULL)
    , tool_type_(ToolType::Pointer)
  {
    tools_[ToolType::Pointer] = new ToolPointer();
    tools_[ToolType::Line] = new ToolLine();
  }


  GraphicsRenderer::~GraphicsRenderer()
  {
    for (int i = 0; i < ToolType::Max; ++i) {
      delete tools_[i];
    }
    for (size_t i = 0; i < graphics_array_.size(); ++i) {
      delete graphics_array_[i];
    }
    graphics_array_.clear();
  }

  bool GraphicsRenderer::SetFBO(GLFramebuffer* fbo)
  {
    out_fbo_ptr_ = fbo;
    return true;
  }

  bool GraphicsRenderer::Run()
  {
    int width = out_fbo_ptr_->GetWidth();
    int height = out_fbo_ptr_->GetHeight();
    out_fbo_ptr_->Bind();

    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -10, 10);

    size_t numb = graphics_array_.size();
    for (size_t i = 0; i < numb; ++i) {
      graphics_array_[i]->Draw();
    }

    out_fbo_ptr_->Unbind();

    return (GL_NO_ERROR == glGetError());
  }

  bool GraphicsRenderer::OnMouseDown(const glm::vec2 &point)
  {
    if (tool_type_ < 0 || tool_type_ >= ToolType::Max
      || tools_[tool_type_] == NULL) return false;

    return tools_[tool_type_]->OnMouseDown(this,
      point);
  }

  bool GraphicsRenderer::OnMouseMove(const glm::vec2 &point)
  {
    if (tool_type_ < 0 || tool_type_ >= ToolType::Max
      || tools_[tool_type_] == NULL) return false;

    return tools_[tool_type_]->OnMouseMove(this,
      point);
  }

  bool GraphicsRenderer::OnMouseUp(const glm::vec2 &point)
  {
    if (tool_type_ < 0 || tool_type_ >= ToolType::Max
      || tools_[tool_type_] == NULL) return false;

    return tools_[tool_type_]->OnMouseUp(this,
      point);
  }

  void GraphicsRenderer::UnselectAll()
  {
    size_t count = graphics_array_.size();
    for (size_t i = 0; i < count; ++i) {
      graphics_array_[i]->SetSelection(false);
    }
  }

  void GraphicsRenderer::RemoveAll()
  {
    graphics_array_.clear();
  }

  void GraphicsRenderer::RemoveSelection()
  {
    std::vector<GraphicsBase*>::iterator iter;
    for (iter = graphics_array_.begin(); iter != graphics_array_.end(); ++iter) {
      if ((*iter)->GetSelection()) {
        delete (*iter);
        graphics_array_.erase(iter);
        return;
      }
    }
  }

  void GraphicsRenderer::SetToolType(ToolType type)
  {
    tool_type_ = type;
  }

  int GraphicsRenderer::GetToolType()
  {
    return tool_type_;
  }
}

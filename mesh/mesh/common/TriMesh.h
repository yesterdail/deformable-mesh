#ifndef HJ_TriMesh_h__
#define HJ_TriMesh_h__

#include "meshext.h"

namespace hj
{
  // Let Point and Normal be a vector made from floats
  typedef OpenMesh::Vec3f Point;
  typedef OpenMesh::Vec3f Vec;
  typedef OpenMesh::Vec3f Normal;

  struct TriMeshTraits : public OpenMesh::DefaultTraits
  {
    VertexAttributes(OpenMesh::Attributes::Normal);
    FaceAttributes(OpenMesh::Attributes::Normal);
  };

  class TriMesh : public OpenMesh::TriMesh_ArrayKernelT<TriMeshTraits>
  {
  public:
    TriMesh(void);
    virtual ~TriMesh(void);

  private:
    Point bbox_min, bbox_max;

  public:
    void needBoundingBox();

    Point getSceneCenter() { return (bbox_min + bbox_max) / 2.0f; };
    float getSceneRadius() { return (bbox_max - bbox_min).norm() / 2.0f; };
    void request_curvature();
    void request_curvature_color();

  public:
    static TriMesh* read(const char* filename, OpenMesh::IO::Options* opt = NULL);
    static bool save(const char* filename, TriMesh* mesh = NULL, OpenMesh::IO::Options* opt = NULL);
  };
}

#endif // HJ_TriMesh_h__
#ifndef hj_meshext_h__
#define hj_meshext_h__


#define _USE_MATH_DEFINES

#include <GL/glew.h>

#include <OpenMesh/Core/Geometry/VectorT.hh>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/IO/Options.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

namespace hj
{
  struct MyTraints : public OpenMesh::DefaultTraits
  {
    HalfedgeAttributes(OpenMesh::Attributes::PrevHalfedge);
  };
  
  typedef OpenMesh::TriMesh_ArrayKernelT<MyTraints> MyMesh;

  inline void glVertex(const hj::MyMesh &mesh, 
    const MyMesh::VertexHandle _vh)
  {
    glVertex3fv(&mesh.point(_vh)[0]);
  }

  inline void glVertex(const MyMesh::Point& _p)
  {
    glVertex3fv(&_p[0]);
  }

  inline void glNormal(const hj::MyMesh &mesh,
    const MyMesh::VertexHandle _vh)
  {
    glNormal3fv(&mesh.normal(_vh)[0]);
  }

  inline void glTexCoord(const hj::MyMesh &mesh,
    const MyMesh::VertexHandle _vh)
  {
    glTexCoord2fv(&mesh.texcoord2D(_vh)[0]);
  }

  inline void glColor(const hj::MyMesh &mesh,
    const MyMesh::VertexHandle _vh)
  {
    glColor3ubv(&mesh.color(_vh)[0]);
  }

  // face properties

  inline void glNormal(const hj::MyMesh &mesh,
    const MyMesh::FaceHandle _fh)
  {
    glNormal3fv(&mesh.normal(_fh)[0]);
  }

  inline void glColor(const hj::MyMesh &mesh,
    const MyMesh::FaceHandle _fh)
  {
    glColor3ubv(&mesh.color(_fh)[0]);
  }

  inline void glMaterial(const hj::MyMesh &mesh,
    const MyMesh::FaceHandle _fh,
    int _f = GL_FRONT_AND_BACK, 
    int _m = GL_DIFFUSE)
  {
    OpenMesh::Vec3f c = OpenMesh::color_cast<OpenMesh::Vec3f>(mesh.color(_fh));
    OpenMesh::Vec4f m(c[0], c[1], c[2], 1.0f);

    glMaterialfv(_f, _m, &m[0]);
  }
}

#endif // hj_meshext_h__

#include "TriMesh.h"

namespace hj
{
  TriMesh::TriMesh(void)
  {
  }

  TriMesh::~TriMesh(void)
  {
  }

  void TriMesh::needBoundingBox()
  {
    TriMesh::ConstVertexIter vIt = vertices_begin();
    TriMesh::ConstVertexIter vEnd = vertices_end();

    bbox_min = bbox_max = point(vIt);

    for (; vIt != vEnd; ++vIt) {
      const Point& p = point(vIt);
      bbox_min.minimize(p);
      bbox_max.maximize(p);
    }
  }

  void TriMesh::request_curvature()
  {
    OpenMesh::VPropHandleT<TriMesh::Point> curvature;
    get_property_handle(curvature, "curvature");
    TriMesh::VertexIter v_it, v_end(vertices_end());
    TriMesh::VertexVertexIter vv_it, vv_it2;
    TriMesh::Scalar voronoiArea;
    for (v_it = vertices_begin(); v_it != v_end; ++v_it)
    {
      property(curvature, v_it)[0] = 0;
      property(curvature, v_it)[1] = 0;
      property(curvature, v_it)[2] = 0;
      voronoiArea = 0;
      for (vv_it = vv_iter(v_it); vv_it.is_valid(); ++vv_it)
      {
        //         (p)  -  p3
        //        /    \     /  
        //      p1 -  p2
        // p is v_it; p2 is vv_it;
        Point p = point(v_it);
        Point p2 = point(vv_it);
        vv_it--;
        Point p3 = point(vv_it);
        vv_it++;
        vv_it++;
        Point p1 = point(vv_it);
        vv_it--;
        TriMesh::Scalar wij = ((p - p1) | (p2 - p1)) / ((p - p1) % (p2 - p1)).length() +
          ((p - p3) | (p2 - p3)) / ((p - p3) % (p2 - p3)).length();
        property(curvature, v_it) += wij * (p2 - p);
        voronoiArea += wij * (p2 - p).length() * (p2 - p).length() / 8;
      }
      property(curvature, v_it) /= 2 * voronoiArea;
    }
  }

  void TriMesh::request_curvature_color()
  {
    OpenMesh::VPropHandleT<TriMesh::Point> curvature;
    get_property_handle(curvature, "curvature");
    OpenMesh::VPropHandleT<TriMesh::Point> curvature_color;
    get_property_handle(curvature_color, "curvature_color");
    TriMesh::VertexIter v_it, v_end(vertices_end());
    std::vector<float> lens;
    TriMesh::Scalar len;
    for (v_it = vertices_begin(); v_it != v_end; ++v_it)
    {
      len = property(curvature, v_it).length();
      lens.push_back(len);
    }
    std::sort(lens.begin(), lens.end());
    TriMesh::Scalar threshold = lens[(unsigned int)ceil(9 * n_vertices() / 10.0)];
    for (v_it = vertices_begin(); v_it != v_end; ++v_it)
    {
      if (property(curvature, v_it).length() > threshold)
        len = 1;
      else
        len = 0;
      property(curvature_color, v_it)[0] = 1 - len;
      property(curvature_color, v_it)[1] = 1 - len;
      property(curvature_color, v_it)[2] = 1 - len;
    }
    lens.clear();
  }

  TriMesh* TriMesh::read(const char* filename, OpenMesh::IO::Options* opt)
  {
    TriMesh* mesh = new TriMesh;

    OpenMesh::IO::Options default_opt;
    if (!opt)
      opt = &default_opt;

    if (!OpenMesh::IO::read_mesh(*mesh, filename, *opt)) {
      delete mesh;
      return NULL;
    }

    // If the file did not provide vertex normals, then calculate them
    if (!opt->check(OpenMesh::IO::Options::VertexNormal) &&
      mesh->has_face_normals() && mesh->has_vertex_normals()) {
      // let the mesh update the normals
      mesh->update_normals();
    }
    // add additional properties
    OpenMesh::VPropHandleT<TriMesh::Point> curvature;
    mesh->add_property(curvature, "curvature");
    OpenMesh::VPropHandleT<TriMesh::Point> curvature_color;
    mesh->add_property(curvature_color, "curvature_color");

    return mesh;
  }

  bool TriMesh::save(const char* filename, TriMesh* mesh, OpenMesh::IO::Options* opt)
  {
    OpenMesh::IO::Options default_opt;
    if (!opt)
      opt = &default_opt;

    if (!OpenMesh::IO::write_mesh(*mesh, filename, *opt))
      return false;
    return true;
  }
}
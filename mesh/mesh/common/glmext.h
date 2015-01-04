#ifndef glm_ext_h__
#define glm_ext_h__

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace glm
{
  template <typename T, precision P>
  GLM_FUNC_DECL T hmul(detail::tvec3<T, P> const & v)
  {
    return v[0] * v[1] * v[2];
  }

  template <typename T, precision P>
  GLM_FUNC_DECL void get(detail::tmat4x4<T, P> const & v, float mat[16])
  {
    mat[0] = (float)v[0][0];
    mat[1] = (float)v[0][1];
    mat[2] = (float)v[0][2];
    mat[3] = (float)v[0][3];
    mat[4] = (float)v[1][0];
    mat[5] = (float)v[1][1];
    mat[6] = (float)v[1][2];
    mat[7] = (float)v[1][3];
    mat[8] = (float)v[2][0];
    mat[9] = (float)v[2][1];
    mat[10] = (float)v[2][2];
    mat[11] = (float)v[2][3];
    mat[12] = (float)v[3][0];
    mat[13] = (float)v[3][1];
    mat[14] = (float)v[3][2];
    mat[15] = (float)v[3][3];
  }

  template <typename T, precision P>
  GLM_FUNC_DECL void get(detail::tmat4x4<T, P> const & v, double mat[16])
  {
    mat[0] = (double)v[0][0];
    mat[1] = (double)v[0][1];
    mat[2] = (double)v[0][2];
    mat[3] = (double)v[0][3];
    mat[4] = (double)v[1][0];
    mat[5] = (double)v[1][1];
    mat[6] = (double)v[1][2];
    mat[7] = (double)v[1][3];
    mat[8] = (double)v[2][0];
    mat[9] = (double)v[2][1];
    mat[10] = (double)v[2][2];
    mat[11] = (double)v[2][3];
    mat[12] = (double)v[3][0];
    mat[13] = (double)v[3][1];
    mat[14] = (double)v[3][2];
    mat[15] = (double)v[3][3];
  }

  template <typename T, precision P>
  GLM_FUNC_DECL detail::tvec3<T, P> transformPoint(detail::tmat4x4<T, P> const & m,detail::tvec3<T, P> const & p)
  {
    detail::tvec4<T, P> point = detail::tvec4<T, P>(p, 1);
    return detail::tvec3<T, P>(m * point);
  }

  template <typename T, precision P>
  GLM_FUNC_DECL detail::tvec3<T, P> transformVector(detail::tmat4x4<T, P> const & mat,detail::tvec3<T, P> const & v)
  {
    detail::tvec4<T, P> vec = detail::tvec4<T, P>(v, 0);
    return detail::tvec3<T, P>(mat * vec);
  }
}

#endif // glm_ext_h__

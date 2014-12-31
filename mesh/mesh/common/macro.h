#ifndef HJ_macro_h__
#define HJ_macro_h__

#include <stdint.h>

namespace hj
{

  // Defines floating point precision and operation.
  const static float kPI = 3.1415926535897932384626433832795f;
  const static float kEpsilonFloat = 1e-6f;
  const static float kDegreeToRadian = 0.017453292519943295f;
  const static float kRadianToDegree = 1.0f / 0.017453292519943295f;

#define deg2rad(x) x*(kPI/180.0f)
#define rad2deg(x) x*(180.0f/kPI)

#define DEL_ARRAY(arr)  if(arr){delete []arr; arr=nullptr;}
#define DEL_PTR(p)      if(p){delete p; p=nullptr;}

  template<class T>
  T MAX(T a, T b) { return a > b ? a : b; }

  template<class T>
  T MIN(T a, T b) { return a < b ? a : b; }
}

#endif // HJ_macro_h__

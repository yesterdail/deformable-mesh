#ifndef HJ_macro_h__
#define HJ_macro_h__

#include <stdint.h>

namespace hj
{

  // Defines floating point precision and operation.
  const static double kPI = 3.1415926535897932384626433832795;
  const static double kEpsilonFloat = 1e-6;
  const static double kEpsilonDouble = 1e-12;
  const static double kDegreeToRadian = 0.017453292519943295;
  const static double kRadianToDegree = 1.0 / 0.017453292519943295;

#define deg2rad(x) x*(kPI/180.0)
#define rad2deg(x) x*(180.0/kPI)

#define DEL_ARRAY(arr)  if(arr){delete []arr; arr=nullptr;}
#define DEL_PTR(p)      if(p){delete p; p=nullptr;}

  template<class T>
  T MAX(T a, T b) { return a > b ? a : b; }

  template<class T>
  T MIN(T a, T b) { return a < b ? a : b; }
}

#endif // HJ_macro_h__

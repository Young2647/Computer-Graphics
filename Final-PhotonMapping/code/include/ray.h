#ifndef CS171_HW3_INCLUDE_RAY_H_
#define CS171_HW3_INCLUDE_RAY_H_
#include <core.h>
/**
 * Data structure representing a ray
 */
struct Ray {
  /* Original point of the ray */
  vec3 origin;
  /* Direction of the ray */
  vec3 direction;
  /* The maximum and minimum value of the parameter t */
  Float tMin;
  Float tMax;

  explicit Ray(const vec3 &origin, const vec3 &direction, Float tMin = 0,
               Float tMax = INF)
      : origin(origin),
        direction(direction.normalized()),
        tMin(tMin),
        tMax(tMax) {}

  [[nodiscard]] vec3 operator()(Float t) const { return getPoint(t); }
  /**
   * get the point at t in the ray
   * @param[in] t a parameter t
   * @return the point
   */
  [[nodiscard]] vec3 getPoint(Float t) const { return origin + t * direction; }
};

#endif  // CS171_HW3_INCLUDE_RAY_H_

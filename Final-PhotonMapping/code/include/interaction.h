#ifndef CS171_HW3_INCLUDE_INTERACTION_H_
#define CS171_HW3_INCLUDE_INTERACTION_H_
#include <core.h>

/**
 * Data structure representing interaction between objects and rays
 */
struct Interaction {
  enum Type {
    NONE,      // No interaction
    GEOMETRY,  // Interaction happens on a geometry
    LIGHT      // Interaction happens on a light source
  };

  // Distance (in units of t) to intersection point
  Float entryDist;
  // Position of intersection point
  vec3 entryPoint;
  // Normal of intersection point (if existed)
  vec3 normal;
  // UV coordinate of intersection point (if existed)
  vec2 uv;
  // Phong lighting model at the intersected point (if existed)
  std::shared_ptr<BRDF> brdf;
  /* Direction of incoming radiance */
  Eigen::Vector3f wi;
  /* Direction of outcoming radiance */
  Eigen::Vector3f wo;
  // Type of interacting object
  Type type;

  Interaction() : entryDist(-1), type(Type::NONE) {}
};

#endif  // CS171_HW3_INCLUDE_INTERACTION_H_

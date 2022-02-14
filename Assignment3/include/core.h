#ifndef CS171_HW3_INCLUDE_CORE_CORE_H_
#define CS171_HW3_INCLUDE_CORE_CORE_H_
#include <Eigen/Core>
#include <Eigen/Dense>
#include <limits>
#include <iostream>
#include <cassert>
#include <memory>
#include <vector>

#ifdef FLOAT_AS_DOUBLE
using Float = double;
#else
using Float = float;
#endif

#define UNIMPLEMENTED                            \
  do {                                           \
    std::cerr << "not implemented" << std::endl; \
    assert(false);                               \
  } while (0)

constexpr Float INF = std::numeric_limits<Float>::infinity();
constexpr Float SHADOW_EPS = static_cast<Float>(1e-4);
constexpr Float PI = static_cast<Float>(3.14159265358979323846);
constexpr Float INV_PI = static_cast<Float>(0.31830988618379067154);
constexpr Float SPEC_STRENGTH = static_cast<Float>(0.50f);
constexpr int SAMPLE_NUM = static_cast<int>(16);
constexpr int PIXSAMPLE_NUM = static_cast<int>(2);
constexpr int SHININESS = static_cast<int>(32);
template <typename T>
using Vector3 = Eigen::Matrix<T, 3, 1>;
template <typename T>
using Vector2 = Eigen::Matrix<T, 2, 1>;

using vec3f = Vector3<Float>;
using vec3 = vec3f;
using vec2f = Vector2<Float>;
using vec2 = vec2f;
using vec2i = Vector2<int>;

using Matrix4x4 = Eigen::Matrix<Float, 4, 4>;

struct Film;
struct Ray;
class Camera;
class Integrator;
struct InteractionPhongModel;
struct Interaction;
class Light;
class AreaLight;
class Material;
class ConstantMaterial;
class TextureMaterial;
class Texture;

/**
 * convert degrees to radians
 * @param[in] x angle in degrees
 * @return x in radians
 */
inline Float radians(Float x) { return x * PI / 180; }

#endif  // CS171_HW3_INCLUDE_CORE_CORE_H_

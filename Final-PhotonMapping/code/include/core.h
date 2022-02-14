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
constexpr int PIXSAMPLE_NUM = static_cast<int>(16);
constexpr Float DEFAULT_PRR = static_cast<Float>(4.0 / 5.0);
constexpr int MAX_DEPTH = static_cast<int>(8);
constexpr int BOUNCD_MAX_TIME = static_cast<int>(10);
constexpr Float DEFAULT_DELTA = static_cast<Float>(1e-3);
constexpr Float VACCUM_IOR = static_cast < Float>(1.0f);
constexpr Float WATER_IOR = static_cast <Float>(1.333f);
constexpr Float GLASS_IOR = static_cast <Float>(1.55f);
constexpr Float DIAMOND_IOR = static_cast <Float>(2.42f);

//constexpr for photon tracing
constexpr int DEFAULT_RENDER_ROUND = static_cast <int>(10);
constexpr int DEFAULT_PHOTON_NUM = static_cast <int>(20000);
constexpr Float DEFAULT_INITIAL_RAIUS = static_cast <int>(5);
constexpr Float DEFAULT_RE_DECAY = static_cast <Float>(0.8);


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
struct Interaction;
class Light;
class AreaLight;
class BRDF;


/**
 * convert degrees to radians
 * @param[in] x angle in degrees
 * @return x in radians
 */
inline Float radians(Float x) { return x * PI / 180; }

#endif  // CS171_HW3_INCLUDE_CORE_CORE_H_

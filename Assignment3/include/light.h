#ifndef CS171_HW3_INCLUDE_LIGHT_H_
#define CS171_HW3_INCLUDE_LIGHT_H_
#include <core.h>
#include <ray.h>
#include <interaction.h>
#include <geometry.h>

// (position, color)
using LightSamplePair = std::pair<vec3, vec3>;
/**
 * Base class of lights
 */
class Light {
 protected:
  vec3 position;
  vec3 color;

 public:
  explicit Light(const vec3 &position, const vec3 &color);
  [[nodiscard]] vec3 getPosition() const;
  [[nodiscard]] vec3 getColor() const;
  /**
   * generate light samples in (position, color) pairs
   * @return light samples
   */
  [[nodiscard]] virtual std::vector<LightSamplePair> samples() const = 0;
  /**
   * ray-light intersect
   * @param[out] interaction output info
   * @param[in] ray the given ray
   * @return whether ray hit the light
   */
  virtual bool intersect(Interaction &interaction, const Ray &ray) const = 0;
};

/**
 * Area lights
 * Only support rectangle lights (normal: (0, -1, 0))
 */
class AreaLight : public Light {
 protected:
  // rectangle -> two triangles
  std::vector<std::shared_ptr<Geometry>> geoms;
  vec2 areaSize;

 public:
  explicit AreaLight(const vec3 &position, const vec3 &color, const vec2 &size);
  /**
   * generate area light samples in (position, color) pairs
   * @return light samples
   */
  [[nodiscard]] std::vector<LightSamplePair> samples() const override;
  /**
   * ray-light intersect
   * @param[out] interaction output info
   * @param[in] ray the given ray
   * @return whether ray hit the light
   */
  bool intersect(Interaction &interaction, const Ray &ray) const override;
};

std::shared_ptr<Light> makeAreaLight(const vec3 &position, const vec3 &color,
                                     const vec2 &size);

#endif  // CS171_HW3_INCLUDE_LIGHT_H_

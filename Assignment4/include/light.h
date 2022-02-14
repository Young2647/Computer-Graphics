#ifndef CS171_HW3_INCLUDE_LIGHT_H_
#define CS171_HW3_INCLUDE_LIGHT_H_
#include <core.h>
#include <ray.h>
#include <interaction.h>
#include <geometry.h>

/**
 * Base class of lights
 */
class Light {
 protected:
  vec3 position;
  vec3 radiance;

 public:
  explicit Light(const vec3 &position, const vec3 &radiance);
  [[nodiscard]] vec3 getPosition() const;
  [[nodiscard]] vec3 getRadiance() const;
  /* Get the emission at the specified position along the given direction */
  virtual vec3 emission(vec3 pos, vec3 dir) = 0;
  /* Sample a position on the light and obtain the corresponding PDF */
  virtual vec3 sample(Interaction &ref_it, Float *pdf) = 0;
  /* Compute the PDF of the given light sample */
  virtual Float pdf(const Interaction &ref_it, vec3 pos) = 0;
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
  vec3 emission(vec3 pos, vec3 dir) override;
  vec3 sample(Interaction &refIt, Float *pdf) override;
  Float pdf(const Interaction &ref_it, vec3 pos) override;
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

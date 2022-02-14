#ifndef CS171_HW3_INCLUDE_SCENE_H_
#define CS171_HW3_INCLUDE_SCENE_H_
#include <light.h>
#include <geometry.h>

class Scene {
 protected:
  std::vector<std::shared_ptr<Geometry>> geometries;
  std::shared_ptr<Light> light;
  std::shared_ptr<Geometry> accel{};
  bool hasAccel{};

 public:
  Scene();
  explicit Scene(std::shared_ptr<Light> light);
  /**
   * add geometries into the scene
   */
  void addGeometry(std::shared_ptr<Geometry> geom);
  void addGeometry(const std::vector<std::shared_ptr<Geometry>> &geoms);
  /**
   * @return returns the number of geometries in the scene
   */
  [[nodiscard]] int countGeometries() const;
  /**
   * sets a light source
   */
  void setLight(std::shared_ptr<Light> newLight);
  /**
   * @return retrieves the light source
   */
  [[nodiscard]] std::shared_ptr<Light> getLight() const;

  /**
   * @param[in] ray the given ray
   * @param[out] interaction output intersect info
   * @return checks whether a ray intersectes with the scene
   */
  bool intersect(const Ray &ray, Interaction &interaction) const;
  /**
   * @param[in] ray the given ray
   * @return checks whether a ray is shadowed in the scene
   */
  [[nodiscard]] bool isShadowed(const Ray &ray) const;

  void buildAccel();
};

#endif  // CS171_HW3_INCLUDE_SCENE_H_

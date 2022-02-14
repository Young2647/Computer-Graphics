#ifndef CS171_HW3_INCLUDE_INTEGRATOR_H_
#define CS171_HW3_INCLUDE_INTEGRATOR_H_
#include <core.h>
#include <utils.h>
#include <scene.h>
#include <camera.h>
#include <integrator.h>

/**
 * Base class of integrator
 */
class Integrator {
 public:
  explicit Integrator(std::shared_ptr<Camera> camera);
  virtual ~Integrator() = default;
  /**
   * render a scene
   * @param[in] the given scene
   */
  virtual void render(Scene &scene) = 0;
  /**
   * calculate the radiance with given scene, ray and interaction
   * @param[in] scene the given scene
   * @param[in] interaction the given interaction
   * @param[in] the given ray
   */
  virtual vec3 radiance(Scene &scene, const Ray &ray, int depth = 0) const = 0;

 protected:
  std::shared_ptr<Camera> camera;
};

/**
 * Phong lighting integrator
 */
class PathIntegrator : public Integrator {
 public:
  PathIntegrator(std::shared_ptr<Camera> camera);
  /**
   * render a scene
   * @param[in] the given scene
   */
  void render(Scene &scene) override;
  /**
   * according to phong lighting model
   * calculate the radiance with given scene, ray and interaction
   * @param[in] scene the given scene
   * @param[in] interaction the given interaction
   * @param[in] the given ray
   */
  vec3 radiance(Scene &scene, const Ray &ray, int depth = 0) const override;
};

std::shared_ptr<Integrator> makePathIntegrator(std::shared_ptr<Camera> camera);

#endif  // CS171_HW3_INCLUDE_INTEGRATOR_H_

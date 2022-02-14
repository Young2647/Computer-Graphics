#include <scene.h>
/**
 * Scene class
 */
Scene::Scene() : light(nullptr) {}

Scene::Scene(std::shared_ptr<Light> light) : light(light) {}

void Scene::addGeometry(std::shared_ptr<Geometry> geom) {
  geometries.push_back(geom);
}

int Scene::countGeometries() const {
  return static_cast<int>(geometries.size());
}

void Scene::setLight(std::shared_ptr<Light> newLight) { light = newLight; }

std::shared_ptr<Light> Scene::getLight() const { return light; }

void Scene::setAmbientLight(const vec3 &ambient) { ambientLight = ambient; }

vec3 Scene::getAmbientLight() const { return ambientLight; }

bool Scene::intersect(const Ray &ray, Interaction &interaction) const {
  Interaction surfaceInteraction;
  light->intersect(surfaceInteraction, ray);
  for (auto &geom : geometries) {
    Interaction curInteraction;
    if (geom->intersect(curInteraction, ray)) {
      if (surfaceInteraction.entryDist == -1 ||
          curInteraction.entryDist < surfaceInteraction.entryDist) {
        surfaceInteraction = curInteraction;
      }
    }
  }

  interaction = surfaceInteraction;
  if (surfaceInteraction.entryDist != -1 &&
      surfaceInteraction.entryDist >= ray.tMin &&
      surfaceInteraction.entryDist <= ray.tMax) {
    return true;
  }
  return false;
}

bool Scene::isShadowed(const Ray &ray) const {
  Interaction in;
  return intersect(ray, in) && in.type == Interaction::Type::GEOMETRY;
}

void Scene::addGeometry(const std::vector<std::shared_ptr<Geometry>> &geoms) {
  for (auto &i : geoms) addGeometry(i);
}

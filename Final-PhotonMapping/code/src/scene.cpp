#include <scene.h>
#include <accel.h>
/**
 * Scene class
 */
Scene::Scene() : light(nullptr), accel(), hasAccel() {}

Scene::Scene(std::shared_ptr<Light> light) : light(light) {}

void Scene::addGeometry(std::shared_ptr<Geometry> geom) {
  geometries.push_back(geom);
}

int Scene::countGeometries() const {
  return static_cast<int>(geometries.size());
}

void Scene::setLight(std::shared_ptr<Light> newLight) { light = newLight; }

std::shared_ptr<Light> Scene::getLight() const { return light; }

bool Scene::intersect(const Ray &ray, Interaction &interaction) const {
  Interaction surfaceInteraction;
  light->intersect(surfaceInteraction, ray);
  if (hasAccel) {
    Interaction curInteraction;
    if (accel->intersect(curInteraction, ray)) {
      if (surfaceInteraction.entryDist == -1 ||
          curInteraction.entryDist < surfaceInteraction.entryDist) {
        surfaceInteraction = curInteraction;
      }
    }
  } else {
    for (auto &geom : geometries) {
      Interaction curInteraction;
      if (geom->intersect(curInteraction, ray)) {
        if (surfaceInteraction.entryDist == -1 ||
            curInteraction.entryDist < surfaceInteraction.entryDist) {
          surfaceInteraction = curInteraction;
        }
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

void Scene::buildAccel() {
  if (geometries.empty()) return;
  accel = std::make_shared<KdTreeAccel>(
      *(reinterpret_cast<std::vector<std::shared_ptr<Triangle>> *>(&geometries)));
  hasAccel = true;
}

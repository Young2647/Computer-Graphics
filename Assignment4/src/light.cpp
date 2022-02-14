#include <light.h>
#include <utils.h>

/**
 * Light class
 */
Light::Light(const vec3 &position, const vec3 &radiance)
    : position(position), radiance(radiance) {}

vec3 Light::getPosition() const { return position; }

vec3 Light::getRadiance() const { return radiance; }

/**
 * AreaLight class
 */
AreaLight::AreaLight(const vec3 &position, const vec3 &color, const vec2 &size)
    : Light(position, color),
      areaSize(size),
      geoms(makeParallelogram(position - vec3(size[0], 0, size[1]) / 2,
                              vec3(size[0], 0, 0), vec3(0, 0, size[1]),
                              vec3(0, -1, 0), nullptr)) {}

vec3 AreaLight::emission(vec3 pos, vec3 dir) {
    auto geom = geoms[0];
    if (dir.dot(geom.get()->getNormal()) > 0.0f)
        return radiance;
    else 
        return vec3::Zero();
    //return std::max(dir.dot(geom.get()->getNormal()), 0.0f) * radiance;
}

Float AreaLight::pdf(const Interaction &ref_it, vec3 pos) {
    auto geom = geoms[0];
    Float cos_theta1 = abs(geom.get()->getNormal().dot(ref_it.wi));
    Float distance = (pos - ref_it.entryPoint).norm();
    return cos_theta1 / (distance * distance); // solid angle of the tiny surface
}

vec3 AreaLight::sample(Interaction &refIt, Float *pdf) {
    auto random_sample = unif(0.0, 1.0, 2);
    Float x1 = random_sample[0];
    Float z1 = random_sample[1];
    vec3 sample_position = position + ((x1 - 0.5) * areaSize[0] * vec3(1, 0, 0)) + ((z1 - 0.5) * areaSize[1] * vec3(0, 0, 1));//uniformly ramdom choose a point on arealight
    refIt.wi = (refIt.entryPoint - sample_position).normalized();// incoming radiance direction
    *pdf = 1 / (areaSize[0] * areaSize[1]); //pdf_light = 1/A
    return sample_position;

}

bool AreaLight::intersect(Interaction &interaction, const Ray &ray) const {
  bool intersection = false;
  for (auto &i : geoms)
    intersection = intersection || i->intersect(interaction, ray);
  interaction.type = Interaction::Type::LIGHT;
  return intersection;
}

std::shared_ptr<Light> makeAreaLight(const vec3 &position, const vec3 &color,
                                     const vec2 &size) {
  return std::make_shared<AreaLight>(position, color, size);
}
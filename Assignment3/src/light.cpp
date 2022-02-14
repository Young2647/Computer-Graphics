#include <light.h>

/**
 * Light class
 */
Light::Light(const vec3 &position, const vec3 &color)
    : position(position), color(color) {}

vec3 Light::getPosition() const { return position; }

vec3 Light::getColor() const { return color; }

/**
 * AreaLight class
 */
AreaLight::AreaLight(const vec3 &position, const vec3 &color, const vec2 &size)
    : Light(position, color),
      areaSize(size),
      geoms(makeParallelogram(position - vec3(size[0], 0, size[1]) / 2,
                              vec3(size[0], 0, 0), vec3(0, 0, size[1]),
                              vec3(0, -1, 0), nullptr)) {}

std::vector<LightSamplePair> AreaLight::samples() const {
	std::vector<LightSamplePair> ret;

	int sample_num = SAMPLE_NUM;
	Float width = areaSize[0];
	Float height = areaSize[1];
	for (int i = 0; i < sample_num; i++)
	{
		for (int j = 0; j < sample_num; j++)
		{
			Float x = (2 * (Float)(i + 0.5)/(Float)sample_num - 1) * 0.5 * width;
			Float y = (2 * (Float)(j + 0.5)/(Float)sample_num - 1) * 0.5 * height;
			vec3 sample_pos = this->getPosition() + x * vec3(1, 0, 0) + y * vec3(0, 0, 1);
			ret.push_back(LightSamplePair(sample_pos,getColor()/(Float)(sample_num * sample_num)));
		}
	}

	return ret;
}

bool AreaLight::intersect(Interaction &interaction,
                                const Ray &ray) const {
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
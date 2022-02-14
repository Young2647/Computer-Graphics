#include <integrator.h>
#include <material.h>
#include <light.h>

/**
 * Integrator class
 */
Integrator::Integrator(std::shared_ptr<Camera> camera) : camera(camera) {}

/**
 * PhongLightingIntegrator class
 */
PhongLightingIntegrator::PhongLightingIntegrator(std::shared_ptr<Camera> camera)
    : Integrator(camera) {}

/**
 * render a scene
 * @param[in] the given scene
 */
void PhongLightingIntegrator::render(Scene &scene) {
  	int now = 0;
#pragma omp parallel for schedule(guided, 2) default(none) shared(now)
  	for (int dx = 0; dx < camera->getFilm().resolution.x(); ++dx) {
#pragma omp atomic
		++now;
		printf("\r%.02f%%", now * 100.0 / camera->getFilm().resolution.x());
		for (int dy = 0; dy < camera->getFilm().resolution.y(); ++dy) {
			vec3 L(0, 0, 0);
			
			std::vector<vec2> pixel_samples = camera->samplePixel(dx, dy);
			for (vec2 pixel_point : pixel_samples)
			{
				Ray cam_ray = camera->generateRay(pixel_point.x(), pixel_point.y());
				Interaction camray_interaction;
				if (scene.intersect(cam_ray, camray_interaction))
				{
					L += radiance(scene, camray_interaction, cam_ray)/pixel_samples.size();
				}
			}
			
			/*
			Ray cam_ray = camera->generateRay((Float)dx + 0.5f, (Float)dy + 0.5f);
			Interaction camray_interaction;
			if (scene.intersect(cam_ray, camray_interaction))
			{
				L = radiance(scene, camray_interaction, cam_ray);
			}
			*/
			// TODO: generate camera ray & calculate radiance
			camera->setPixel(dx, dy, L);
		}
	}
}

/**
 * according to phong lighting model
 * calculate the radiance with given scene, ray and interaction
 * @param[in] scene the given scene
 * @param[in] interaction the given interaction
 * @param[in] the given ray
 */
vec3 PhongLightingIntegrator::radiance(Scene &scene,
                                       const Interaction &interaction,
                                       const Ray &ray) const {
	if (interaction.type == interaction.NONE)
	{
		return vec3::Zero();
	}
	else if (interaction.type == interaction.LIGHT)
	{
		return scene.getLight()->getColor();
	}
	else if (interaction.type == interaction.GEOMETRY)
	{
		vec3 ambient = (scene.getAmbientLight().array() * interaction.lightingModel.ambient.array()).matrix();
		vec3 diffuse = vec3::Zero();
		vec3 specular = vec3::Zero();
		for (LightSamplePair pointLight : scene.getLight()->samples())
		{
			///Diffuse
			vec3 lightDir = (pointLight.first - interaction.entryPoint).normalized();
			Ray lightRay(interaction.entryPoint, lightDir);
			if (!scene.isShadowed(lightRay))
			{
				///Diffuse
				Float diff = std::max(interaction.normal.dot(lightDir), 0.0f);
				Eigen::Array3f temp_diff = diff * pointLight.second.array() * interaction.lightingModel.diffusion.array();
				diffuse += temp_diff.matrix();

				///Specluar
				vec3 reflectDir = (2 * lightDir.dot(interaction.normal) * interaction.normal - lightDir).normalized();
				Float spec = (Float)std::pow(std::max(reflectDir.dot(-ray.direction), 0.0f), interaction.lightingModel.shininess);
				Eigen::Array3f temp_spec =  spec * pointLight.second.array() * interaction.lightingModel.specular.array();
				specular += temp_spec.matrix();
			}
		}
		//return interaction.normal * 0.5 + vec3(0.5, 0.5, 0.5);
		return ambient + diffuse + specular;
	}
	return vec3::Zero();
}

std::shared_ptr<Integrator> makePhongLightingIntegrator(
    std::shared_ptr<Camera> camera) {
  return std::make_shared<PhongLightingIntegrator>(camera);
}
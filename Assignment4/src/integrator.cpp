#include <integrator.h>
#include <brdf.h>
#include <light.h>
#define USE_OPENMP 1
/**
 * Integrator class
 */
Integrator::Integrator(std::shared_ptr<Camera> camera) : camera(camera) {}

/**
 * PhongLightingIntegrator class
 */
PathIntegrator::PathIntegrator(std::shared_ptr<Camera> camera)
    : Integrator(camera) {}

/**
 * render a scene
 * @param[in] the given scene
 */
void PathIntegrator::render(Scene &scene) {
  int now = 0;
#ifdef USE_OPENMP
#pragma omp parallel for schedule(guided, 16) default(none) shared(now)
#endif
  for (int dx = 0; dx < camera->getFilm().resolution.x(); ++dx) {
#ifdef USE_OPENMP
#pragma omp atomic
#endif
    ++now;
    printf("\r%.02f%%", now * 100.0 / camera->getFilm().resolution.x());
    for (int dy = 0; dy < camera->getFilm().resolution.y(); ++dy) {
      vec3 L(0, 0, 0);

      // TODO: anti-aliasing
      std::vector<vec2> pixel_samples = camera->samplePixel(dx, dy);
      for (vec2 pixel_point : pixel_samples)
      {
            Ray cam_ray = camera->generateRay(pixel_point.x(), pixel_point.y());
            L += radiance(scene,cam_ray) / pixel_samples.size();
      }
      //Ray ray = camera->generateRay(dx, dy);
      //L += radiance(scene, ray);
      camera->setPixel(dx, dy, L);
    }
  }
}

bool checkInvalid(vec3& vec)
{
    if (isnan(vec.x()) || isnan(vec.y()) || isnan(vec.z())) return true;
    if (isinf(vec.x()) || isinf(vec.y()) || isinf(vec.z())) return true;
    return false;
}

bool checktooLarge(vec3& vec)
{
    Float max_emission = 1.0f;
    if (vec.x() > max_emission || vec.y() > max_emission || vec.z() > max_emission) return true;
    return false;
}
/**
 * calculate the radiance with given scene, ray and interaction
 * @param[in] scene the given scene
 * @param[in] interaction the given interaction
 * @param[in] the given ray
 */
vec3 PathIntegrator::radiance(Scene &scene, const Ray &ray, int depth) const {
    vec3 L(0, 0, 0);
    if (depth >= MAX_DEPTH) return L;
    Interaction interaction;
    if (!scene.intersect(ray, interaction)) return L;
  
    if (interaction.type == Interaction::Type::NONE) return L;

    interaction.wo = -ray.direction;
    if (interaction.type == Interaction::Type::LIGHT)
    {
        //return scene.getLight()->getRadiance();
        return scene.getLight()->emission(interaction.entryPoint, interaction.wo);
    }
    if (interaction.type == Interaction::Type::GEOMETRY)
    {
        //computing direct radiance
        Float light_pdf = 0.0f;
        vec3 direct_radiance = vec3::Zero();
        vec3 light_sample_position = scene.getLight()->sample(interaction, &light_pdf);
        Ray light_ray(interaction.entryPoint, -interaction.wi, SHADOW_EPS, INF);
        if (!scene.isShadowed(light_ray))
        {
            Float solid_angle = scene.getLight()->pdf(interaction, light_sample_position);
            Float cos_theta = abs(interaction.normal.dot(-interaction.wi));
            vec3 L_i = scene.getLight()->emission(interaction.entryPoint, interaction.wi);
            vec3 f_r = interaction.brdf.get()->eval(interaction);
            direct_radiance = vecMul(L_i, f_r) * cos_theta * solid_angle / light_pdf ;
        }
        if (interaction.brdf.get()->isDelta()) //multiple importance sampling
        {
            Float brdf_pdf = 0.0f;
            vec3 brdf_direct_radiance = vec3::Zero();
            brdf_pdf = interaction.brdf.get()->sample(interaction);
            Ray brdf_ray(interaction.entryPoint, -interaction.wi, SHADOW_EPS, INF);
            if (!scene.isShadowed(brdf_ray))
            {
                vec3 L_i = scene.getLight()->emission(interaction.entryPoint, interaction.wi);
                vec3 f_r = interaction.brdf.get()->eval(interaction);
                Float cos_theta = abs(interaction.normal.dot(-interaction.wi));
                brdf_direct_radiance = vecMul(L_i, f_r) * cos_theta / brdf_pdf;
            }
            // balance heuristic
            direct_radiance = ((light_pdf * light_pdf) / (brdf_pdf * brdf_pdf + light_pdf * light_pdf)) * direct_radiance + ((brdf_pdf * brdf_pdf) / (brdf_pdf * brdf_pdf + light_pdf * light_pdf)) * brdf_direct_radiance;
        }
        if (checkInvalid(direct_radiance)) direct_radiance = vec3::Zero();
        //computing indirect radiance
        vec3 indirect_radiance = vec3::Zero();
        
        //we use Russian Roulette to stop recursion
        //auto test_RR = unif(0.0, 1.0, 1);
        //Float P_RR = DEFAULT_PRR;
        //if (test_RR[0] > P_RR) // RR done, stop recursion
        //{
        //    return direct_radiance + indirect_radiance;
        //}

        Float wi_pdf = interaction.brdf.get()->sample(interaction);
        Ray next_ray(interaction.entryPoint, -interaction.wi, SHADOW_EPS, INF);
        if (!scene.isShadowed(next_ray)) // the next ray hit light directly
        {
            if (checktooLarge(direct_radiance)) return vec3::Zero();
            return direct_radiance;
        }
        //vec3 L_i = vec3(0, 0, 0);
        vec3 L_i = radiance(scene, next_ray, depth + 1);
        vec3 f_r = interaction.brdf.get()->eval(interaction);
        Float cos_theta = abs(interaction.normal.dot(-interaction.wi));
        indirect_radiance = vecMul(L_i, f_r) * cos_theta / wi_pdf;
        if (checkInvalid(indirect_radiance)) indirect_radiance = vec3::Zero();
        vec3 tmp = direct_radiance + indirect_radiance;
        if (checkInvalid(tmp)) return vec3::Zero();
        if (checktooLarge(tmp)) return vec3::Zero();
        return direct_radiance + indirect_radiance;
    }
    // ---------------
    return L;
}

std::shared_ptr<Integrator> makePathIntegrator(std::shared_ptr<Camera> camera) {
  return std::make_shared<PathIntegrator>(camera);
}

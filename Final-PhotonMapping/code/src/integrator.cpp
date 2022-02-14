#include <integrator.h>
#include <brdf.h>
#include <light.h>
#include <iostream>

#define USE_OPENMP 1
/**
 * Integrator class
 */
Integrator::Integrator(std::shared_ptr<Camera> camera) : camera(camera) {}

/**
 * PhongLightingIntegrator class
 */
PathIntegrator::PathIntegrator(std::shared_ptr<Camera> camera)
    : Integrator(camera) {  }




/**
 * render a scene
 * @param[in] the given scene
 */
void PathIntegrator::render(Scene& scene) {
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
            //Ray ray = camera->generateRay(dx, dy);
            //L += radiance(scene, ray);
            camera->setPixel(dx, dy, L);
        }
    }
}


/**
 * calculate the radiance with given scene, ray and interaction
 * @param[in] scene the given scene
 * @param[in] interaction the given interaction
 * @param[in] the given ray
 */
vec3 PathIntegrator::radiance(Scene &scene, const Ray &ray, int depth) const {
    return vec3::Zero();
}


/// <summary>
/// Integrator for photon mapping
/// </summary>
/// <param name="round"></param>
PhotonIntegrator::PhotonIntegrator(std::shared_ptr<Camera> camera)
    : Integrator(camera), render_round(DEFAULT_RENDER_ROUND), 
    photon_num(DEFAULT_PHOTON_NUM), initial_radius(DEFAULT_INITIAL_RAIUS), re_decay(DEFAULT_RE_DECAY), max_depth(MAX_DEPTH),
    bounceMaxDepth(BOUNCD_MAX_TIME){}

PhotonIntegrator::PhotonIntegrator(std::shared_ptr<Camera> camera, int render_round, int photon_num, Float initial_radius,
    Float re_decay, int bounceMaxDepth, int max_depth)
    : Integrator(camera)
{
    this->render_round = render_round;
    this->photon_num = photon_num;
    this->initial_radius = initial_radius;
    this->re_decay = re_decay;
    this->bounceMaxDepth = bounceMaxDepth;
    this->max_depth = max_depth;
}

vec3 PhotonIntegrator::radiance(Scene& scene, const Ray& ray, int depth) const {
    return vec3::Zero();
}

void PhotonIntegrator::setRenderround(int round)
{
    render_round = round;
}

void PhotonIntegrator::buildKdPointTree(std::vector<std::shared_ptr<ViewPoint>> viewpoints)
{
    this->kd_point_tree = std::make_shared<KdPointTree>(KdPointTree(viewpoints));
}


void PhotonIntegrator::RayTracing(Scene& scene, const Ray& ray, double strength, int x, int y, int depth = 0)
{
  if (depth >= bounceMaxDepth) return;
  Ray new_ray = ray;
  Interaction interaction;
  if (scene.intersect(new_ray, interaction)) {
    if (interaction.type) {
      interaction.wo = -new_ray.direction;
      if (interaction.type == Interaction::GEOMETRY) {
        if (strcmp(interaction.brdf->getName(), "IdealDiffusion") == 0)
        {
          std::shared_ptr<ViewPoint> new_point(new ViewPoint(interaction.entryPoint, interaction.normal, interaction.brdf->eval(interaction), strength, x, y));
#pragma omp critical
          view_points.push_back(new_point);
        }
        else
        {
          interaction.brdf->sample(interaction);
          new_ray.direction = interaction.wi;
          new_ray.origin = interaction.entryPoint + 0.0001 * new_ray.direction;
          return RayTracing(scene, new_ray, strength, x, y, depth+1);
        }
      }
      else if (interaction.type == Interaction::LIGHT) {
        vec3 tempL = scene.getLight()->emission(interaction.entryPoint, new_ray.direction);
#pragma omp critical
        pixels_data[x * camera->getFilm().resolution.y() + y] = tempL;
        //std::shared_ptr<ViewPoint> new_point(new ViewPoint(interaction.entryPoint, interaction.normal, vec3(0, 0, 0), 0.0, 0, 0));
//#pragma omp critical
        //view_points.push_back(new_point);s
      }
    }
  }
}

void PhotonIntegrator::render(Scene& scene) {
    //initialize for render process
    scene.buildAccel();
    int now = 0;
    int film_x = camera->getFilm().resolution.x();
    int film_y = camera->getFilm().resolution.y();

    pixels_data.resize(film_x * film_y);//initialize pixels data
    
    std::vector<int> samples_count; // to count how manys samples in each pixel
    samples_count.resize(film_x * film_y);
    samples_count.assign(samples_count.size(), 0); // initialize it to zero
    
    Float current_radius = initial_radius; //initialize radius for photon_tracing
    Float current_energy = 1.0f / log(render_round); //initialize energy for photon tracing

    for (int dx = 0; dx < camera->getFilm().resolution.x(); ++dx)
    {
        for (int dy = 0; dy < camera->getFilm().resolution.y(); ++dy)
        {
            camera->setPixel(dx, dy, vec3::Zero());
        }
    }

    // start rendering
    for (int iter = 0; iter < render_round; iter++)
    {
        // clear pixels_data every round
        view_points.clear(); //clear last round view points
        pixels_data.assign(pixels_data.size(), vec3::Zero()); 
        

#ifdef USE_OPENMP
#pragma omp parallel for schedule(guided, 16) default(none) shared(now,scene,current_energy,samples_count,film_y)
#endif
        for (int dx = 0; dx < camera->getFilm().resolution.x(); ++dx)
        {
#ifdef USE_OPENMP
#pragma omp atomic
#endif
            ++now;
            printf("\r%.02f%%", now * 100.0 / camera->getFilm().resolution.x());
            for (int dy = 0; dy < camera->getFilm().resolution.y(); ++dy)
            {
                vec3 L(0, 0, 0);
                Float _dx = dx + (unif(0.0, 1.0 ,1)[0] * 1.0 - .5) * 1;
                Float _dy = dy + (unif(0.0, 1.0, 1)[0] * 1.0 - .5) * 1; // add random interruption every round
                Ray cam_ray = camera->generateRay(_dx, _dy);
                RayTracing(scene, cam_ray, current_energy, dx, dy);
                samples_count[dx * film_y + dy]++;
                // TODO: anti-aliasing
                //Ray ray = camera->generateRay(dx, dy);
                //L += radiance(scene, ray);
                //camera->setPixel(dx, dy, L);
            }
        }
        
        buildKdPointTree(view_points);
        printf("\nPhoton rendering...");
        int photon_now = 0;
#ifdef USE_OPENMP
#pragma omp parallel for schedule(guided, 16) default(none) shared(photon_now)
#endif
        for (int i = 0; i < photon_num; i++)
        {
            printf("\r%.02f%%", photon_now * 100.0 / photon_num);
            vec3 light_energy;
            Ray light_ray = scene.getLight()->generateRay(light_energy); // randomly generate a ray from light
            //vec3 radi=light_energy/photon_num;
            vec3 radi = scene.getLight()->getRadiance()/ photon_num;
            PhotonTracing(scene,light_ray,1,radi,current_radius); // todo
#ifdef USE_OPENMP
#pragma omp atomic
#endif
            photon_now++;
        }


        for (int dx = 0; dx < camera->getFilm().resolution.x(); ++dx)
        {
            for (int dy = 0; dy < camera->getFilm().resolution.y(); ++dy)
            {
                camera->updatePixel(dx, dy, pixels_data[dx * film_y + dy] * 1/ samples_count[dx* film_y + dy]); //update pixel every time
            }
        }
        std::string file_name = "output_round";
        file_name += std::to_string(iter);
        file_name += ".png";
        camera->getFilm().write(file_name);
        current_radius *= re_decay; //the radius decreases every round
        current_energy /= re_decay; //the accumulated energy increases every round
    }

}

std::shared_ptr<Integrator> makePathIntegrator(std::shared_ptr<Camera> camera) {
  return std::make_shared<PathIntegrator>(camera);
}

std::shared_ptr<Integrator> makePhotonIntegrator(std::shared_ptr<Camera> camera) {
    return std::make_shared<PhotonIntegrator>(camera);
}

std::shared_ptr<Integrator> makePhotonIntegrator(std::shared_ptr<Camera> camera, int render_round, int photon_num, Float initial_radius, 
        Float re_decay, int bouncemaxdepth, int max_depth) {
    return std::make_shared<PhotonIntegrator>(camera, render_round, photon_num, initial_radius, re_decay, bouncemaxdepth, max_depth);
}

void PhotonIntegrator::PhotonTracing(Scene& scene, const Ray& ray, const int depth, const vec3 radi, const Float current_radius){
  if (depth > max_depth) return;
  Interaction interact;
  scene.intersect(ray, interact);
  if (interact.type != Interaction::GEOMETRY) return;
  interact.wo = -ray.direction;
  if(interact.brdf->isDelta())
  {
    interact.brdf->sample(interact);
    vec3 newdir = interact.wi;
    Ray newray = Ray(interact.entryPoint + 0.0001 * newdir, newdir);
    PhotonTracing(scene,newray,depth+1,radi,current_radius);
  }
  else
  {
    std::vector<std::shared_ptr<ViewPoint>> tmpViewPoint;
    kd_point_tree->search(tmpViewPoint,interact.entryPoint,current_radius);
    for(auto &v:tmpViewPoint){
      if(v->N.dot(ray.direction) < 0)
      {
        Float r = (current_radius - (v->C - interact.entryPoint).norm()) / current_radius;
        vec3 res = v->color.cwiseProduct(radi).cwiseMax(vec3::Zero()) * r * r * v->strength;
#pragma omp critical
        pixels_data[v->x * camera->getFilm().resolution.y() + v->y] += res;
        
      }
    }
    interact.brdf->sample(interact);
    Ray newray = Ray(interact.entryPoint + 0.0001 * interact.wi, interact.wi);
    vec3 tmp_color = interact.brdf->eval(interact);
    tmp_color = tmp_color.cwiseProduct(radi).cwiseMax(vec3::Zero()) * PI;
    PhotonTracing(scene, newray, depth + 1, tmp_color, current_radius);
  }
}

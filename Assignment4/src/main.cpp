#include <integrator.h>
#include <geometry.h>
#include <obj_loader.h>
#include <chrono>
#include <cstdint>

std::shared_ptr<Camera> genCamera(int id = 0,
                                  const vec2i &res = vec2i(512, 512));
std::shared_ptr<Scene> genCornellBoxScene(int id = 0);

int main(int argc, const char *argv[]) {
  int sceneId = 1;
  if (argc > 1) {
    int id = std::stoi(argv[1]);
    if (0 <= id && id <= 4) sceneId = id;
  }
  auto camera = genCamera(sceneId);
  auto scene = genCornellBoxScene(sceneId);
  // uncomment this after finishing k-d tree
  scene->buildAccel();

  // Perform Phong lighting integrator
  std::cout << "Rendering..." << std::endl;

  auto start = std::chrono::high_resolution_clock::now();
  auto integrator = makePathIntegrator(camera);
  integrator->render(*scene);
  auto end = std::chrono::high_resolution_clock::now();
  double timeElapsed = static_cast<double>(
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
          .count());
  std::cout << std::endl
            << "Time elapsed: " << timeElapsed << " ms" << std::endl;

  // Output the image to a file
  camera->getFilm().write("output.png");

  return 0;
}

std::shared_ptr<Camera> genCamera(int id, const vec2i &res) {
  vec3 cameraPos;
  vec3 cameraLookAt;
  Float fov = 45;
  if (id == 0 || id == 1 || id == 2 || id == 3 || id == 4) {
    cameraPos = vec3(0, 1, 6.8);
    cameraLookAt = vec3(0, 1, 0);
    fov = 19.5;
  }
  auto camera = makeCamera(cameraPos, fov, res);
  camera->lookAt(cameraLookAt, vec3(0, 1, 0));
  return camera;
}

std::shared_ptr<Scene> genCornellBoxScene(int id) {
  // Material settings
  auto leftWallMat = makeIdealDiffusion(vec3(0.630000, 0.065000, 0.050000));
  auto rightWallMat = makeIdealDiffusion(vec3(0.140000, 0.450000, 0.091000));
  auto floorMat = makeIdealDiffusion(vec3(0.725000, 0.710000, 0.680000));
  auto ceilingMat = makeIdealDiffusion(vec3(0.725000, 0.710000, 0.680000));
  auto backWallMat = makeIdealDiffusion(vec3(0.725000, 0.710000, 0.680000));
  //auto shortBoxMat = makeIdealDiffusion(vec3(0.725000, 0.710000, 0.680000));
  //auto shortBoxMat = makeIdealSpecular(vec3(0.725000, 0.710000, 0.680000));
  auto shortBoxMat = makeIdealTransmission(vec3(1.0, 1.0, 1.0), GLASS_IOR);
  //auto shortBoxMat = makeGlossyMetal(vec3(1.0, 1.0, 1.0), 0.1);
  //auto tallBoxMat = makeIdealSpecular(vec3(1.0, 1.0, 1.0));
  //auto tallBoxMat = makeGlossyMetal(vec3(1.0,1.0,1.0), 0.1);
  //auto tallBoxMat = makeIdealDiffusion(vec3(0.725000, 0.710000, 0.680000));
  auto tallBoxMat = makeIdealTransmission(vec3(1.0, 1.0, 1.0), GLASS_IOR);
  auto makeVec2 = [](const std::vector<Float> &vec) {
    std::vector<vec2> ret;
    for (int i = 0; i < static_cast<int>(vec.size()); i += 2) {
      ret.emplace_back(vec[i], vec[i + 1]);
    }
    return ret;
  };
  auto makeVec3 = [](const std::vector<Float> &vec) {
    std::vector<vec3> ret;
    for (int i = 0; i < static_cast<int>(vec.size()); i += 3) {
      ret.emplace_back(vec[i], vec[i + 1], vec[i + 2]);
    }
    return ret;
  };
  // a box
  // clang-format off
  std::vector<int> index{0, 2, 1, 0, 3, 2, 4, 6, 5, 4, 7, 6, 8, 10, 9, 8, 11, 10, 12, 14, 13, 12, 15, 14, 16, 18, 17, 16, 19, 18, 20, 22, 21, 20, 23, 22};
  std::vector<vec3> pos = makeVec3({-0.720444, 1.2, -0.473882, -0.720444, 0, -0.473882, -0.146892, 0, -0.673479, -0.146892, 1.2, -0.673479, -0.523986, 0, 0.0906493, -0.523986, 1.2, 0.0906492, 0.0495656, 1.2, -0.108948, 0.0495656, 0, -0.108948, -0.523986, 1.2, 0.0906492, -0.720444, 1.2, -0.473882, -0.146892, 1.2, -0.673479, 0.0495656, 1.2, -0.108948, 0.0495656, 0, -0.108948, -0.146892, 0, -0.673479, -0.720444, 0, -0.473882, -0.523986, 0, 0.0906493, -0.523986, 0, 0.0906493, -0.720444, 0, -0.473882, -0.720444, 1.2, -0.473882, -0.523986, 1.2, 0.0906492, 0.0495656, 1.2, -0.108948, -0.146892, 1.2, -0.673479, -0.146892, 0, -0.673479, 0.0495656, 0, -0.108948});
  std::vector<vec3> normals = makeVec3({-0.328669, -4.1283e-008, -0.944445, -0.328669, -4.1283e-008, -0.944445, -0.328669, -4.1283e-008, -0.944445, -0.328669, -4.1283e-008, -0.944445, 0.328669, 4.1283e-008, 0.944445, 0.328669, 4.1283e-008, 0.944445, 0.328669, 4.1283e-008, 0.944445, 0.328669, 4.1283e-008, 0.944445, 3.82137e-015, 1, -4.37114e-008, 3.82137e-015, 1, -4.37114e-008, 3.82137e-015, 1, -4.37114e-008, 3.82137e-015, 1, -4.37114e-008, -3.82137e-015, -1, 4.37114e-008, -3.82137e-015, -1, 4.37114e-008, -3.82137e-015, -1, 4.37114e-008, -3.82137e-015, -1, 4.37114e-008, -0.944445, 1.43666e-008, 0.328669, -0.944445, 1.43666e-008, 0.328669, -0.944445, 1.43666e-008, 0.328669, -0.944445, 1.43666e-008, 0.328669, 0.944445, -1.43666e-008, -0.328669, 0.944445, -1.43666e-008, -0.328669, 0.944445, -1.43666e-008, -0.328669, 0.944445, -1.43666e-008, -0.328669});
  std::vector<vec2> uv = makeVec2({0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1});
  auto tallBox = makeTriangleMesh(index, static_cast<int>(pos.size()), pos, normals, uv, tallBoxMat);

  // shortBox
  pos = makeVec3({-0.0460751, 0.6, 0.573007, -0.0460751, -2.98023e-008, 0.573007, 0.124253, 0, 0.00310463, 0.124253, 0.6, 0.00310463, 0.533009, 0, 0.746079, 0.533009, 0.6, 0.746079, 0.703337, 0.6, 0.176177, 0.703337, 2.98023e-008, 0.176177, 0.533009, 0.6, 0.746079, -0.0460751, 0.6, 0.573007, 0.124253, 0.6, 0.00310463, 0.703337, 0.6, 0.176177, 0.703337, 2.98023e-008, 0.176177, 0.124253, 0, 0.00310463, -0.0460751, -2.98023e-008, 0.573007, 0.533009, 0, 0.746079, 0.533009, 0, 0.746079, -0.0460751, -2.98023e-008, 0.573007, -0.0460751, 0.6, 0.573007, 0.533009, 0.6, 0.746079, 0.703337, 0.6, 0.176177, 0.124253, 0.6, 0.00310463, 0.124253, 0, 0.00310463, 0.703337, 2.98023e-008, 0.176177});
  normals = makeVec3({-0.958123, -4.18809e-008, -0.286357, -0.958123, -4.18809e-008, -0.286357, -0.958123, -4.18809e-008, -0.286357, -0.958123, -4.18809e-008, -0.286357, 0.958123, 4.18809e-008, 0.286357, 0.958123, 4.18809e-008, 0.286357, 0.958123, 4.18809e-008, 0.286357, 0.958123, 4.18809e-008, 0.286357, -4.37114e-008, 1, -1.91069e-015, -4.37114e-008, 1, -1.91069e-015, -4.37114e-008, 1, -1.91069e-015, -4.37114e-008, 1, -1.91069e-015, 4.37114e-008, -1, 1.91069e-015, 4.37114e-008, -1, 1.91069e-015, 4.37114e-008, -1, 1.91069e-015, 4.37114e-008, -1, 1.91069e-015, -0.286357, -1.25171e-008, 0.958123, -0.286357, -1.25171e-008, 0.958123, -0.286357, -1.25171e-008, 0.958123, -0.286357, -1.25171e-008, 0.958123, 0.286357, 1.25171e-008, -0.958123, 0.286357, 1.25171e-008, -0.958123, 0.286357, 1.25171e-008, -0.958123, 0.286357, 1.25171e-008, -0.958123});
  auto shortBox = makeTriangleMesh(index, static_cast<int>(pos.size()), pos, normals, uv, shortBoxMat);
  // clang-format on

  // the Cornell box geometry
  index = {0, 1, 2, 0, 2, 3};
  pos = makeVec3({-1, 0, -1, -1, 0, 1, 1, -0, 1, 1, -0, -1});
  normals = makeVec3({0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0});
  uv = makeVec2({0, 0, 1, 0, 1, 1, 0, 1});
  auto floor = makeTriangleMesh(index, static_cast<int>(pos.size()), pos,
                                normals, uv, floorMat);

  pos = makeVec3({1, 2, 1, -1, 2, 1, -1, 2, -1, 1, 2, -1});
  normals = makeVec3({0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0});
  auto ceiling = makeTriangleMesh(index, static_cast<int>(pos.size()), pos,
                                  normals, uv, ceilingMat);

  pos = makeVec3({-1, 0, -1, -1, 2, -1, 1, 2, -1, 1, 0, -1});
  normals = makeVec3({0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1});
  auto backWall = makeTriangleMesh(index, static_cast<int>(pos.size()), pos,
                                   normals, uv, backWallMat);

  pos = makeVec3({1, 0, -1, 1, 2, -1, 1, 2, 1, 1, 0, 1});
  normals = makeVec3({-1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0});
  auto rightWall = makeTriangleMesh(index, static_cast<int>(pos.size()), pos,
                                    normals, uv, rightWallMat);

  pos = makeVec3({-1, 0, 1, -1, 2, 1, -1, 2, -1, -1, 0, -1});
  normals = makeVec3({1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0});
  auto leftWall = makeTriangleMesh(index, static_cast<int>(pos.size()), pos,
                                   normals, uv, leftWallMat);

  std::vector<std::shared_ptr<Geometry>> bunny;
  std::vector<std::shared_ptr<Geometry>> dragon;
  std::vector<std::vector<std::shared_ptr<Geometry>>> sub;

  if (id == 1) {
    //bunny = makeObjMesh("assets/stanford_bunny.obj", shortBoxMat, 4,
    //                    vec3(0.5, 0, 0));
    bunny = makeObjMesh("assets/sphere.obj", shortBoxMat, 1,
        vec3(0, 0, 0));
  }
  if (id == 2 || id == 3 || id == 4) {
    dragon = makeObjMesh("assets/stanford_dragon.obj", shortBoxMat, 5,
                         vec3(0, 0.3, 0));
  }

  if (id == 3) {
    sub.reserve(7);
    for (int i = 0; i < 7; ++i) {
      vec3 trans(cos(radians(static_cast<float>(i) / 7.0f * 360.0f)), 0,
                 sin(radians(static_cast<float>(i) / 7.0f * 360.0f)));
      trans *= 0.6f;
      sub.emplace_back(
          makeObjMesh("assets/stanford_bunny.obj", shortBoxMat, 2, trans));
    }
  }

  if (id == 4) {
    sub.reserve(7);
    for (int i = 0; i < 7; ++i) {
      vec3 trans(cos(radians(static_cast<float>(i) / 7.0f * 360.0f)), 0,
                 sin(radians(static_cast<float>(i) / 7.0f * 360.0f)));
      trans *= 0.6f;
      sub.emplace_back(
          makeObjMesh("assets/stanford_dragon.obj", shortBoxMat, 2, trans));
    }
  }

  // Light setting
  vec3 lightPos;
  vec3 lightColor;
  vec2 lightSize;

  lightPos = vec3(-0.005, 1.98, -0.03);
  lightColor = vec3(17.0, 12.0, 4.0);
  lightSize = vec2(0.47, 0.38);
  auto light = makeAreaLight(lightPos, lightColor, lightSize);

  // Scene setup
  auto scene = std::make_shared<Scene>(light);
  scene->addGeometry(backWall);
  scene->addGeometry(leftWall);
  scene->addGeometry(rightWall);
  scene->addGeometry(floor);
  scene->addGeometry(ceiling);
  if (id == 0 || id == 1) scene->addGeometry(tallBox);
  if (id == 0) scene->addGeometry(shortBox);
  if (id == 1) scene->addGeometry(bunny);
  if (id == 2 || id == 3 || id == 4) scene->addGeometry(dragon);
  if (id == 3 || id == 4)
    for (auto &m : sub) scene->addGeometry(m);
  return scene;
}

#include <integrator.h>
#include <geometry.h>

#include <chrono>
#include <cstdint>
#include <cstdio>

std::string getPath(const std::string& target, int depth = 5) {
    std::string path = target;
    for (int i = 0; i < depth; ++i) {
        FILE* file = fopen(path.c_str(), "r");
        if (file) {
            fclose(file);
            return path;
        }
        path = "../" + path;
    }
    return target;
}

std::shared_ptr<Camera> genCamera(int id = 0,
                                  const vec2i &res = vec2i(512, 512));
std::shared_ptr<Scene> genCornellBoxScene(int lightId = 0);

int main(int argc, const char *argv[]) {
  int sceneId = 0;
  if (argc > 1) {
    int id = std::stoi(argv[1]);
    if (0 <= id && id <= 2) sceneId = id;
  }
  auto camera = genCamera(sceneId);
  auto scene = genCornellBoxScene(sceneId);

  // Perform Phong lighting integrator
  std::cout << "Rendering..." << std::endl;

  auto start = std::chrono::high_resolution_clock::now();
  auto integrator = makePhongLightingIntegrator(camera);
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
  if (id == 0) {
    cameraPos = vec3(0, 0, 10);
    cameraLookAt = vec3(0, 0, 0);
  } else if (id == 1) {
    cameraPos = vec3(5, 3, 12);
    cameraLookAt = vec3(0, 0, 0);
  } else if (id == 2) {
    cameraPos = vec3(0, -4, 15);
    cameraLookAt = vec3(0, 0, 0);
  }
  Float fov = 45;
  auto camera = makeCamera(cameraPos, fov, res);
  camera->lookAt(cameraLookAt, vec3(0, 1, 0));
  return camera;
}

std::shared_ptr<Scene> genCornellBoxScene(int lightId) {

  // Material settings
  
  auto grayMat = makeConstantMaterial(vec3(0.7, 0.7, 0.7), 16.0);
  auto WallMat = makeTextureMaterial(getPath("textures/brickwall.jpg"), 16.0);
  WallMat.get()->loadNormaltexture(getPath("textures/brickwall_normal.jpg"));
  auto redMat = makeConstantMaterial(vec3(1.0, 0, 0), 16.0);
  auto greenMat = makeConstantMaterial(vec3(0, 1.0, 0), 16.0);
  //auto boxMat = makeConstantMaterial(vec3(0.725000, 0.710000, 0.680000), 16.0);
  auto boxMat = makeTextureMaterial(getPath("textures/colorful_texture.jpg"), 16.0);
  // the Cornell box geometry
  vec3 p0(-10, -10, -10);
  vec3 s0(1, 0, 0);
  vec3 s1(0, 1, 0);
  vec3 normal(0, 0, 1);
  auto backWall = makeParallelogram(p0, s0 * 20, s1 * 20, normal, WallMat);

  p0 = vec3(-6, -7, -11);
  s0 = vec3(0, 0, 1);
  s1 = vec3(0, 1, 0);
  normal = vec3(1, 0, 0);
  auto leftWall = makeParallelogram(p0, s0 * 20, s1 * 20, normal, WallMat);

  p0 = vec3(6, -7, -11);
  s0 = vec3(0, 0, 1);
  s1 = vec3(0, 1, 0);
  normal = vec3(-1, 0, 0);
  auto rightWall = makeParallelogram(p0, s0 * 20, s1 * 20, normal, WallMat);

  p0 = vec3(-7, -6, -11);
  s0 = vec3(1, 0, 0);
  s1 = vec3(0, 0, 1);
  normal = vec3(0, 1, 0);
  auto floor = makeParallelogram(p0, s0 * 20, s1 * 20, normal, WallMat);

  p0 = vec3(-7, 6, -11);
  s0 = vec3(1, 0, 0);
  s1 = vec3(0, 0, 1);
  normal = vec3(0, -1, 0);
  auto ceiling = makeParallelogram(p0, s0 * 20, s1 * 20, normal, WallMat);

  // a box
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
  // clang-format off
  std::vector<int> index{0, 2, 1, 0, 3, 2, 4, 6, 5, 4, 7, 6, 8, 10, 9, 8, 11, 10, 12, 14, 13, 12, 15, 14, 16, 18, 17, 16, 19, 18, 20, 22, 21, 20, 23, 22};
  std::vector<vec3> pos = makeVec3({-0.720444, 1.2, -0.473882, -0.720444, 0, -0.473882, -0.146892, 0, -0.673479, -0.146892, 1.2, -0.673479, -0.523986, 0, 0.0906493, -0.523986, 1.2, 0.0906492, 0.0495656, 1.2, -0.108948, 0.0495656, 0, -0.108948, -0.523986, 1.2, 0.0906492, -0.720444, 1.2, -0.473882, -0.146892, 1.2, -0.673479, 0.0495656, 1.2, -0.108948, 0.0495656, 0, -0.108948, -0.146892, 0, -0.673479, -0.720444, 0, -0.473882, -0.523986, 0, 0.0906493, -0.523986, 0, 0.0906493, -0.720444, 0, -0.473882, -0.720444, 1.2, -0.473882, -0.523986, 1.2, 0.0906492, 0.0495656, 1.2, -0.108948, -0.146892, 1.2, -0.673479, -0.146892, 0, -0.673479, 0.0495656, 0, -0.108948});
  std::vector<vec3> normals = makeVec3({-0.328669, -4.1283e-008, -0.944445, -0.328669, -4.1283e-008, -0.944445, -0.328669, -4.1283e-008, -0.944445, -0.328669, -4.1283e-008, -0.944445, 0.328669, 4.1283e-008, 0.944445, 0.328669, 4.1283e-008, 0.944445, 0.328669, 4.1283e-008, 0.944445, 0.328669, 4.1283e-008, 0.944445, 3.82137e-015, 1, -4.37114e-008, 3.82137e-015, 1, -4.37114e-008, 3.82137e-015, 1, -4.37114e-008, 3.82137e-015, 1, -4.37114e-008, -3.82137e-015, -1, 4.37114e-008, -3.82137e-015, -1, 4.37114e-008, -3.82137e-015, -1, 4.37114e-008, -3.82137e-015, -1, 4.37114e-008, -0.944445, 1.43666e-008, 0.328669, -0.944445, 1.43666e-008, 0.328669, -0.944445, 1.43666e-008, 0.328669, -0.944445, 1.43666e-008, 0.328669, 0.944445, -1.43666e-008, -0.328669, 0.944445, -1.43666e-008, -0.328669, 0.944445, -1.43666e-008, -0.328669, 0.944445, -1.43666e-008, -0.328669});
  std::vector<vec2> uv = makeVec2({0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1});
  for (auto &v : pos) v = (v * 4 - vec3(1.7, 6.0, 6));
  auto box = makeTriangleMesh(index, static_cast<int>(pos.size()), pos, normals, uv, WallMat);

  // shortBox
  pos = makeVec3({-0.0460751, 0.6, 0.573007, -0.0460751, -2.98023e-008, 0.573007, 0.124253, 0, 0.00310463, 0.124253, 0.6, 0.00310463, 0.533009, 0, 0.746079, 0.533009, 0.6, 0.746079, 0.703337, 0.6, 0.176177, 0.703337, 2.98023e-008, 0.176177, 0.533009, 0.6, 0.746079, -0.0460751, 0.6, 0.573007, 0.124253, 0.6, 0.00310463, 0.703337, 0.6, 0.176177, 0.703337, 2.98023e-008, 0.176177, 0.124253, 0, 0.00310463, -0.0460751, -2.98023e-008, 0.573007, 0.533009, 0, 0.746079, 0.533009, 0, 0.746079, -0.0460751, -2.98023e-008, 0.573007, -0.0460751, 0.6, 0.573007, 0.533009, 0.6, 0.746079, 0.703337, 0.6, 0.176177, 0.124253, 0.6, 0.00310463, 0.124253, 0, 0.00310463, 0.703337, 2.98023e-008, 0.176177});
  normals = makeVec3({-0.958123, -4.18809e-008, -0.286357, -0.958123, -4.18809e-008, -0.286357, -0.958123, -4.18809e-008, -0.286357, -0.958123, -4.18809e-008, -0.286357, 0.958123, 4.18809e-008, 0.286357, 0.958123, 4.18809e-008, 0.286357, 0.958123, 4.18809e-008, 0.286357, 0.958123, 4.18809e-008, 0.286357, -4.37114e-008, 1, -1.91069e-015, -4.37114e-008, 1, -1.91069e-015, -4.37114e-008, 1, -1.91069e-015, -4.37114e-008, 1, -1.91069e-015, 4.37114e-008, -1, 1.91069e-015, 4.37114e-008, -1, 1.91069e-015, 4.37114e-008, -1, 1.91069e-015, 4.37114e-008, -1, 1.91069e-015, -0.286357, -1.25171e-008, 0.958123, -0.286357, -1.25171e-008, 0.958123, -0.286357, -1.25171e-008, 0.958123, -0.286357, -1.25171e-008, 0.958123, 0.286357, 1.25171e-008, -0.958123, 0.286357, 1.25171e-008, -0.958123, 0.286357, 1.25171e-008, -0.958123, 0.286357, 1.25171e-008, -0.958123});
  std::vector<vec3> floatingBoxPos = pos;
  for (auto &v : pos) v = (v * 6 - vec3(-1, 6, 9));
  auto shortBox = makeTriangleMesh(index, static_cast<int>(pos.size()), pos, normals, uv, WallMat);
  pos = floatingBoxPos;
  for (auto &v : pos) v = (v * 2 - vec3(2, 0, 7));
  auto floatingBox = makeTriangleMesh(index, static_cast<int>(pos.size()), pos, normals, uv, boxMat);
  // clang-format on

  // Light setting
  vec3 lightPos;
  vec3 lightColor;

  if (lightId == 0) {
    lightPos = vec3(0, 5.9, -6);
    lightColor = vec3(1.0, 1.0, 1.0);
  } else if (lightId == 1) {
    lightPos = vec3(3, 5.9, -6);
    lightColor = vec3(0.9, 0.8, 0);
  } else if (lightId == 2) {
    lightPos = vec3(-3, 5.9, -6);
    lightColor = vec3(0, 0.9, 0.7);
  }
  auto light = makeAreaLight(lightPos, lightColor, vec2(2, 2));

  // Scene setup
  auto scene = std::make_shared<Scene>(light);
  scene->setAmbientLight(vec3(0.1, 0.1, 0.1));
  scene->addGeometry(backWall);
  scene->addGeometry(leftWall);
  scene->addGeometry(rightWall);
  scene->addGeometry(floor);
  scene->addGeometry(ceiling);
  scene->addGeometry(box);
  scene->addGeometry(shortBox);
  scene->addGeometry(floatingBox);
  return scene;
}
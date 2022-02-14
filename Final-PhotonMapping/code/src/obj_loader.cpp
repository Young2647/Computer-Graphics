#include <obj_loader.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <iostream>

bool loadObj(const std::string &path, std::vector<vec3> &vertices,
             std::vector<vec2> &texCoords, std::vector<vec3> &normals,
             std::vector<int> &index) {
  std::clog << "-- Loading model " << path << std::endl;

  tinyobj::ObjReaderConfig readerConfig;
  readerConfig.mtl_search_path = "./";  // Path to material files

  tinyobj::ObjReader reader;
  std::string input = path;
  for (int dep = 0; dep < 5; ++dep) {
    if (!reader.ParseFromFile(input, readerConfig)) {
      input = "../" + input;
      if (dep < 4) {
        continue;
      }
      if (!reader.Error().empty()) {
        std::clog << "TinyObjReader: " << reader.Error();
      }
      exit(1);
    }
  }

  if (!reader.Warning().empty()) {
    std::cout << "TinyObjReader: " << reader.Warning();
  }

  auto &attrib = reader.GetAttrib();
  auto &shapes = reader.GetShapes();
  auto &materials = reader.GetMaterials();

  for (size_t i = 0; i < attrib.vertices.size(); i += 3) {
    vertices.push_back(vec3(attrib.vertices[i], attrib.vertices[i + 1],
                            attrib.vertices[i + 2]));
  }
  texCoords.resize(vertices.size());
  normals.resize(vertices.size());
  // Loop over shapes
  for (size_t s = 0; s < shapes.size(); s++) {
    // Loop over faces(polygon)
    size_t index_offset = 0;
    for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
      size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

      // Loop over vertices in the face.
      for (size_t v = 0; v < fv; v++) {
        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
        index.push_back(idx.vertex_index);
        if (!attrib.texcoords.empty()) {
          texCoords[idx.vertex_index] =
              vec2(attrib.texcoords[idx.texcoord_index * 2 + 0],
                   attrib.texcoords[idx.texcoord_index * 2 + 1]);
        }
        normals[idx.vertex_index] =
            vec3(attrib.normals[idx.normal_index * 3 + 0],
                 attrib.normals[idx.normal_index * 3 + 1],
                 attrib.normals[idx.normal_index * 3 + 2]).normalized();
      }
      index_offset += fv;
    }
  }

  std::clog << "  # vertices: " << attrib.vertices.size() << std::endl;
  std::clog << "  # faces: " << index.size() / 3 << std::endl;
  return true;
}

std::vector<std::shared_ptr<Geometry>> makeObjMesh(const std::string &path,
                                                   std::shared_ptr<BRDF> mat,
                                                   Float scale,
                                                   vec3 translation) {
  std::vector<vec3> vertices;
  std::vector<vec2> texCoords;
  std::vector<vec3> normals;
  std::vector<int> index;
  loadObj(path, vertices, texCoords, normals, index);
  for (auto &v : vertices) v = v * scale + translation;
  return makeTriangleMesh(index, static_cast<int>(vertices.size()), vertices,
                          normals, texCoords, mat);
}
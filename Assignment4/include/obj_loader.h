#ifndef CS171_HW4_INCLUDE_OBJ_LOADER_H_
#define CS171_HW4_INCLUDE_OBJ_LOADER_H_
#include <core.h>
#include <geometry.h>
#include <iostream>
#include <vector>
#include <memory>

bool loadObj(const std::string &path, std::vector<vec3> &vertices,
             std::vector<vec2> &texCoords, std::vector<vec3> &normals,
             std::vector<int> &index);

std::vector<std::shared_ptr<Geometry>> makeObjMesh(const std::string &path,
                                                   std::shared_ptr<BRDF> mat, Float scale = 1,
                                                   vec3 translation = vec3::Zero());
#endif  // CS171_HW4_INCLUDE_OBJ_LOADER_H_

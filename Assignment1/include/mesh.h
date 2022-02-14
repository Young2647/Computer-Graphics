#ifndef _MESH_H_
#define _MESH_H_
#include "defines.h"
#include <vector>
#include "shader.h"

struct Vertex {
  vec3 position;
  vec3 normal;
};

class Mesh {
 public:
  /**
   * construct a mesh from a obj file
   */
	Mesh();
	Mesh(const std::string &path);
  /**
   * draw mesh objects
   */
  void InitMesh();
  void draw() const;
  void Terminate();
 private:
  //  mesh data
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  unsigned int VAO, VBO;

  void loadDataFromFile(const std::string &path);
};
#endif
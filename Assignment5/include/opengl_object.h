#ifndef _OPENGL_OBJECT_
#define _OPENGL_OBJECT_

#include <vector>

#include <shader.h>
#include <defines.h>

struct Vertex
{
  glm::vec3 position; // Vertex position.
  glm::vec3 normal;   // Vertex normal.
};

struct DrawOption
{
  bool draw_element;
  GLenum primitive_mode;

  DrawOption() : draw_element(true),
                 primitive_mode(GL_TRIANGLES)
  {
  }
};

// Class to define an object for rendering with OpenGL.
class OpenGLObject
{
private:
  unsigned int VAO;
  unsigned int VBO;
  unsigned int EBO;

public:
  glm::vec3 color; // Material color.

  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;

  glm::mat4 M; // Model transformation matrix.

  DrawOption draw_option;

  OpenGLObject();
  ~OpenGLObject() = default;

  void InitData();
  // Draw the current object.
  void Draw(Shader &shader, ShaderParam &shader_param);
};

#endif // _OPENGL_OBJECT_
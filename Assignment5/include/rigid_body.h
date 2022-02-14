#ifndef _RIGID_BODY_
#define _RIGID_BODY_

#include <vector>
#include <shader.h>

#include <opengl_object.h>

// Force applied to a particle point of a rigid body.
struct Force
{
  glm::vec3 r; // Particle position in the local coordinate of the rigid body.
  glm::vec3 F; // Force vector.

  Force() : r(glm::vec3(0.0f, 0.0f, 0.0f)),
            F(glm::vec3(0.0f, 0.0f, 0.0f))
  {
  }

  Force(glm::vec3 r, glm::vec3 F) : r(r), F(F)
  {
  }
  Force back_force()
  {
      Force temp;
      temp.F = -this->F;
      return temp;
  }
};

// Interface calss for rigid bodies.
class RigidBody
{
protected:
  glm::vec3 x; // Center point.

  glm::vec3 v; // Linear velocity.
  glm::vec3 w; // Angular velocity.

  float M;         // Mass.
  glm::mat3 I;     // Inertia tensor.
  glm::mat3 I_inv; // Inverse of Inertia tensor.

  glm::vec3 P; // Linear momentum.
  glm::vec3 L; // Angular momentum.

  OpenGLObject opengl_object; // OpenGL object for rendering.

public:
  std::vector<Force> constant_forces;  // Current applied constant forces which will not disappear.
  std::vector<Force> transient_forces; // Current applied trasient forces which will disapper immediately.

public:
  RigidBody() = default;
  virtual ~RigidBody() = default;

  // Get the protected member variables.
  glm::vec3 GetX();
  glm::vec3 GetV();
  float GetM();

  // Draw the rigib body based on the OpenGLObject.
  void Draw(Shader &shader, ShaderParam &shader_param);
  // Clear the transient forces.
  void ClearTransientForces();
  void ClearSpeedTooSmall();
  // Update the rigid body according to the applied forces.
  virtual void Forward(float dt) = 0;
  virtual void Forward(float dt, bool onlyposition) = 0;
  virtual void Backward(float dt) = 0;
  virtual void Backward(float dt, bool onlyposition) = 0;

protected:
  // Initialize inertia tensor and the inverse tensor.
  virtual void InitInertiaTensor() = 0;
  // Initialize OpenGL object.
  virtual void InitOpenGLObject(glm::vec3 color) = 0;
  // Update the attributes of the OpenGL object, e.g. position of each vertex.
  virtual void UpdateOpenGLObject() = 0;
};

// Class for spheres, inherited from Class RigidBody.
class Sphere : public RigidBody
{
protected:
  float radius;

public:
  Sphere(
      glm::vec3 color,
      glm::vec3 x,
      glm::vec3 v,
      glm::vec3 w,
      float M,
      float radius);
  ~Sphere() = default;

  float GetRadius();

  void Forward(float dt) override;
  void Forward(float dt, bool onlyposition) override;
  void Backward(float dt) override;
  void Backward(float dt, bool onlyposition) override;

protected:
  void InitInertiaTensor() override;
  void InitOpenGLObject(glm::vec3 color) override;

  void UpdateOpenGLObject() override;
};

// Class for walls, inherited from Class RigidBody.
class Wall : public RigidBody
{
protected:
  glm::vec3 s1; // Edge vector 1.
  glm::vec3 s2; // Edge vector 2.

public:
  Wall(
      glm::vec3 color,
      glm::vec3 x,
      glm::vec3 s1,
      glm::vec3 s2);
  ~Wall() = default;

  glm::vec3 GetS1();
  glm::vec3 GetS2();

  void Forward(float dt) override;
  void Forward(float dt, bool onlyposition) override { return; }
  void Backward(float dt) override;
  void Backward(float dt, bool onlyposition) override { return; }

protected:
  void InitInertiaTensor() override;
  void InitOpenGLObject(glm::vec3 color) override;

  void UpdateOpenGLObject() override;
};

#endif // _RIGID_BODY_
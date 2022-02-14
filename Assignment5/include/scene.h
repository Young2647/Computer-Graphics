#ifndef _SCENE_
#define _SCENE_

#include <defines.h>
#include <shader.h>
#include <rigid_body.h>
#include <collision.h>

class Scene
{
protected:
  float dt;                  // Simulation time step.
  float intersect_tolerance; // The intersection threshold.
                             // We only tolerate the collision within this threshold.
                             // Otherwise, we need to adjust the collision position.

public:
  std::vector<Wall> walls; // Walls in the scene.
  std::vector<Sphere> spheres;  // Spheres in the scene.

  CollisionHandler collision_handler; // The collision handler.

  Scene(
      int id,
      float dt,
      float normal_epsilon,
      float tangent_epsilon,
      float intersect_tolerance);
  ~Scene() = default;

  // Draw the current scene.
  void Draw(Shader &shader, ShaderParam &shader_param);
  // Update the elements' states in the current scene.
  void Update();

protected:
  // Update the whole scene forward and backward.
  void Forward(float dt);
  void Forward(float dt, bool onlyposition);
  void Backward(float dt);
  void Backward(float dt, bool onlyposition);

  // Collision helper functions.
  bool IsCollided(float &intersect_degree);
  void AdjustCollision(float &intersect_degree);
  void ApplyCollision();
  void FindRealCollision(float & intersect_degree, float delta_t);
  // Clear the transient forces.
  void ClearTransientForces();
};

#endif // _SCENE_
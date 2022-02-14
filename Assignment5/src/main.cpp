#include <utils.h>
#include <shader.h>
#include <camera.h>

#include <opengl_object.h>
#include <scene.h>

void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double x, double y);

const int WIDTH = 800;
const int HEIGHT = 600;

bool firstMouse = true;
float lastX = WIDTH / 2.0;
float lastY = HEIGHT / 2.0;

bool play = false;

GLFWwindow *window;
Camera mycamera;

int main()
{
  WindowGuard windowGuard(window, WIDTH, HEIGHT, "CS171 hw5");
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_callback);
  glEnable(GL_DEPTH_TEST);

  Shader shader("../src/shaders/projective.vs", "../src/shaders/phong_lighting.fs");
  ShaderParam shader_param;
  shader_param.projection = glm::perspective(glm::radians(45.0f), 1.3f, 0.1f, 100.0f);

  int id = 4;
  Scene scene(id, 0.02f, 0.8f, 0.99f, 1e-3);

  std::cout << "Press on the space key to play." << std::endl;

  while (!glfwWindowShouldClose(window))
  {
    processInput(window);

    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader_param.view = mycamera.getViewMatrix();
    shader_param.cameraPos = mycamera.Position;

    scene.Draw(shader, shader_param);
    if (play)
    {
      scene.Update();
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);

    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  return 0;
}

void processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
  {
    mycamera.processWalkAround(Forward);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
  {
    mycamera.processWalkAround(Backward);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
  {
    mycamera.processWalkAround(Leftward);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
  {
    mycamera.processWalkAround(Rightward);
  }
  if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
  {
    std::cout << "Camera position: (" << mycamera.Position.x << ", "
              << mycamera.Position.y << ", " << mycamera.Position.z << ")" << std::endl;
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
  {
    play = true;
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
  {
    play = false;
  }
}

void mouse_callback(GLFWwindow *window, double x, double y)
{
  x = (float)x;
  y = (float)y;
  if (firstMouse)
  {
    lastX = x;
    lastY = y;
    firstMouse = false;
  }

  mycamera.processLookAround(x - lastX, y - lastY);

  // update record
  lastX = x;
  lastY = y;
}

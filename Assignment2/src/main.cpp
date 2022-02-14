#include <utils.h>
#include <object.h>
#include <shader.h>
#include <bezier.h>
#include <bspline.h>
#include <light.h>
#include <camera.h>
#include <controlpoint.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


void processInput(GLFWwindow* window);
void processDrawState(GLFWwindow* window, float& lastChangetime, float currentFrame, int& state);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double x, double y);
void drawControlPoints(std::vector<Controlpoint> & controlpoints, Shader & shader, mat4 view, mat4 projection);
void showImgui(std::vector<std::vector<vec3>>& controlpoints, std::vector<Controlpoint>& drawpoints, std::vector<std::vector<vec3>>& originpoints, bool & ifchange_);


const int WIDTH = 800;
const int HEIGHT = 600;

const int DrawBezier = 0;
const int DrawBspline = 1;

bool firstMouse = true;
float lastX = WIDTH/2.0;
float lastY = HEIGHT/2.0;
bool ifRightclicked = false;

GLFWwindow* window;
Camera mycamera;
Light light(vec3(3.0f, 2.0f, 0.0f));

int main() {
  WindowGuard windowGuard(window, WIDTH, HEIGHT, "CS171 hw2");
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glEnable(GL_DEPTH_TEST);
  
  
  ///construct Bezier Surfaces
  BezierSurface a_surface(4,4);
  a_surface.setControlPoint(0, 0, vec3(0.0f, 1.0f, -1.0f));
  a_surface.setControlPoint(0, 1, vec3(0.5f, 1.0f, -0.5f));
  a_surface.setControlPoint(0, 2, vec3(0.5f, 1.0f,  0.0f));
  a_surface.setControlPoint(0, 3, vec3(0.0f, 1.0f,  0.5f));

  a_surface.setControlPoint(1, 0, vec3(0.0f, 0.5f, -1.0f));
  a_surface.setControlPoint(1, 1, vec3(1.0f, 0.5f, -0.5f));
  a_surface.setControlPoint(1, 2, vec3(1.0f, 0.5f,  0.0f));
  a_surface.setControlPoint(1, 3, vec3(0.0f, 0.5f,  0.5f));

  a_surface.setControlPoint(2, 0, vec3(0.0f, 0.0f, -1.0f));
  a_surface.setControlPoint(2, 1, vec3(1.0f, 0.0f, -0.5f));
  a_surface.setControlPoint(2, 2, vec3(1.0f, 0.0f,  0.0f));
  a_surface.setControlPoint(2, 3, vec3(0.0f, 0.0f,  0.5f));

  a_surface.setControlPoint(3, 0, vec3(0.0f, -0.5f, -1.0f));
  a_surface.setControlPoint(3, 1, vec3(0.5f, -0.5f, -0.5f));
  a_surface.setControlPoint(3, 2, vec3(0.5f, -0.5f,  0.0f));
  a_surface.setControlPoint(3, 3, vec3(0.0f, -0.5f,  0.5f));

  ///construct object a
  Object a_object = a_surface.generateObject();
  a_object.init();
  ///prepare to draw a_object control points
  std::vector<Controlpoint> a_controlpoints;
  for (int i = 0; i < a_surface.control_points_m_.size(); i++)
  {
    for (int j = 0; j < a_surface.control_points_m_[i].size(); j++)
    {
      a_controlpoints.push_back(Controlpoint(a_surface.control_points_m_[i][j]));
    }
  }


  BezierSurface b_surface(4,4);
  b_surface.setControlPoint(0, 0, vec3(0.0f, 1.0f, -1.0f));
  b_surface.setControlPoint(0, 1, vec3(-0.5f, 1.0f, -1.5f));
  b_surface.setControlPoint(0, 2, vec3(-1.0f, 1.0f,  -1.0f));
  b_surface.setControlPoint(0, 3, vec3(-1.0f, 1.0f,  -0.5f));

  b_surface.setControlPoint(1, 0, vec3(0.0f, 0.5f, -1.0f));
  b_surface.setControlPoint(1, 1, vec3(-1.0f, 0.5f, -1.5f));
  b_surface.setControlPoint(1, 2, vec3(-2.0f, 0.5f,  -1.0f));
  b_surface.setControlPoint(1, 3, vec3(-2.0f, 0.5f,  -0.5f));

  b_surface.setControlPoint(2, 0, vec3(0.0f, 0.0f, -1.0f));
  b_surface.setControlPoint(2, 1, vec3(-1.0f, 0.0f, -1.5f));
  b_surface.setControlPoint(2, 2, vec3(-2.0f, 0.0f,  -1.0f));
  b_surface.setControlPoint(2, 3, vec3(-2.0f, 0.0f,  -0.5f));

  b_surface.setControlPoint(3, 0, vec3(0.0f, -0.5f, -1.0f));
  b_surface.setControlPoint(3, 1, vec3(-0.5f, -0.5f, -1.5f));
  b_surface.setControlPoint(3, 2, vec3(-1.0f, -0.5f,  -1.0f));
  b_surface.setControlPoint(3, 3, vec3(-1.0f, -0.5f,  -0.5f));

  ///construct object b
  Object b_object = b_surface.generateObject();
  b_object.init();

  std::vector<Controlpoint> b_controlpoints;
  for (int i = 0; i < b_surface.control_points_m_.size(); i++)
  {
    for (int j = 0; j < b_surface.control_points_m_[i].size(); j++)
    {
      b_controlpoints.push_back(Controlpoint(b_surface.control_points_m_[i][j]));
    }
  }

  ///construct c_surface
  BezierSurface c_surface(4, 4);
  c_surface.setControlPoint(0, 0, vec3(-1.0f, 1.0f, -0.5f));
  c_surface.setControlPoint(0, 1, vec3(-1.0f, 1.0f, 0.0f));
  c_surface.setControlPoint(0, 2, vec3(-0.5f, 1.0f, 1.0f));
  c_surface.setControlPoint(0, 3, vec3(0.0f, 1.0f, 0.5f));

  c_surface.setControlPoint(1, 0, vec3(-2.0f, 0.5f, -0.5f));
  c_surface.setControlPoint(1, 1, vec3(-2.0f, 0.5f, 0.0f));
  c_surface.setControlPoint(1, 2, vec3(-1.0f, 0.5f, 1.0f));
  c_surface.setControlPoint(1, 3, vec3(0.0f, 0.5f, 0.5f));

  c_surface.setControlPoint(2, 0, vec3(-2.0f, 0.0f, -0.5f));
  c_surface.setControlPoint(2, 1, vec3(-2.0f, 0.0f, 0.0f));
  c_surface.setControlPoint(2, 2, vec3(-1.0f, 0.0f, 1.0f));
  c_surface.setControlPoint(2, 3, vec3(0.0f, 0.0f, 0.5f));

  c_surface.setControlPoint(3, 0, vec3(-1.0f, -0.5f, -0.5f));
  c_surface.setControlPoint(3, 1, vec3(-1.0f, -0.5f, 0.0f));
  c_surface.setControlPoint(3, 2, vec3(-0.5f, -0.5f, 1.0f));
  c_surface.setControlPoint(3, 3, vec3(0.0f, -0.5f, 0.5f));

  ///construct object b
  Object c_object = c_surface.generateObject();
  c_object.init();

  std::vector<Controlpoint> c_controlpoints;
  for (int i = 0; i < c_surface.control_points_m_.size(); i++)
  {
    for (int j = 0; j < c_surface.control_points_m_[i].size(); j++)
    {
      c_controlpoints.push_back(Controlpoint(c_surface.control_points_m_[i][j]));
    }
  }
  
  ///construct Bspline Surface
  BsplineSurface myBsplineSurface(7, 7, 3, 3);
    
  myBsplineSurface.SetKnotM(0, 0);
  myBsplineSurface.SetKnotM(1, 0);
  myBsplineSurface.SetKnotM(2, 0);
  myBsplineSurface.SetKnotM(3, 0);
  myBsplineSurface.SetKnotM(4, 0.25);
  myBsplineSurface.SetKnotM(5, 0.5);
  myBsplineSurface.SetKnotM(6, 0.75);
  myBsplineSurface.SetKnotM(7, 1);
  myBsplineSurface.SetKnotM(8, 1);
  myBsplineSurface.SetKnotM(9, 1);
  myBsplineSurface.SetKnotM(10, 1);

  myBsplineSurface.SetKnotN(0, 0);
  myBsplineSurface.SetKnotN(1, 0);
  myBsplineSurface.SetKnotN(2, 0);
  myBsplineSurface.SetKnotN(3, 0);
  myBsplineSurface.SetKnotN(4, 0.25);
  myBsplineSurface.SetKnotN(5, 0.5);
  myBsplineSurface.SetKnotN(6, 0.75);
  myBsplineSurface.SetKnotN(7, 1);
  myBsplineSurface.SetKnotN(8, 1);
  myBsplineSurface.SetKnotN(9, 1);
  myBsplineSurface.SetKnotN(10, 1);

  myBsplineSurface.Setcontrolpoint(0, 0, vec3(0.0f,1.5f,-1.5f));
  myBsplineSurface.Setcontrolpoint(0, 1, vec3(0.5f,1.5f,-1.0f));
  myBsplineSurface.Setcontrolpoint(0, 2, vec3(1.0f,1.5f,-0.5f));
  myBsplineSurface.Setcontrolpoint(0, 3, vec3(1.5f,1.5f, 0.0f));
  myBsplineSurface.Setcontrolpoint(0, 4, vec3(1.0f,1.5f, 0.5f));
  myBsplineSurface.Setcontrolpoint(0, 5, vec3(0.5f,1.5f, 1.0f));
  myBsplineSurface.Setcontrolpoint(0, 6, vec3(0.0f,1.5f, 1.5f));

  myBsplineSurface.Setcontrolpoint(1, 0, vec3(0.5f,1.0f,-1.5f));
  myBsplineSurface.Setcontrolpoint(1, 1, vec3(1.0f,1.0f,-1.0f));
  myBsplineSurface.Setcontrolpoint(1, 2, vec3(1.5f,1.0f,-0.5f));
  myBsplineSurface.Setcontrolpoint(1, 3, vec3(2.0f,1.0f, 0.0f));
  myBsplineSurface.Setcontrolpoint(1, 4, vec3(1.5f,1.0f, 0.5f));
  myBsplineSurface.Setcontrolpoint(1, 5, vec3(1.0f,1.0f, 1.0f));
  myBsplineSurface.Setcontrolpoint(1, 6, vec3(0.5f,1.0f, 1.5f));

  myBsplineSurface.Setcontrolpoint(2, 0, vec3(1.0f,0.5f,-1.5f));
  myBsplineSurface.Setcontrolpoint(2, 1, vec3(1.5f,0.5f,-1.0f));
  myBsplineSurface.Setcontrolpoint(2, 2, vec3(2.0f,0.5f,-0.5f));
  myBsplineSurface.Setcontrolpoint(2, 3, vec3(2.5f,0.5f, 0.0f));
  myBsplineSurface.Setcontrolpoint(2, 4, vec3(2.0f,0.5f, 0.5f));
  myBsplineSurface.Setcontrolpoint(2, 5, vec3(1.5f,0.5f, 1.0f));
  myBsplineSurface.Setcontrolpoint(2, 6, vec3(1.0f,0.5f, 1.5f));

  myBsplineSurface.Setcontrolpoint(3, 0, vec3(1.5f,0.0f,-1.5f));
  myBsplineSurface.Setcontrolpoint(3, 1, vec3(2.0f,0.0f,-1.0f));
  myBsplineSurface.Setcontrolpoint(3, 2, vec3(2.5f,0.0f,-0.5f));
  myBsplineSurface.Setcontrolpoint(3, 3, vec3(3.0f,0.0f, 0.0f));
  myBsplineSurface.Setcontrolpoint(3, 4, vec3(2.5f,0.0f, 0.5f));
  myBsplineSurface.Setcontrolpoint(3, 5, vec3(2.0f,0.0f, 1.0f));
  myBsplineSurface.Setcontrolpoint(3, 6, vec3(1.5f,0.0f, 1.5f));

  myBsplineSurface.Setcontrolpoint(4, 0, vec3(1.0f,-0.5f,-1.5f));
  myBsplineSurface.Setcontrolpoint(4, 1, vec3(1.5f,-0.5f,-1.0f));
  myBsplineSurface.Setcontrolpoint(4, 2, vec3(2.0f,-0.5f,-0.5f));
  myBsplineSurface.Setcontrolpoint(4, 3, vec3(2.5f,-0.5f, 0.0f));
  myBsplineSurface.Setcontrolpoint(4, 4, vec3(2.0f,-0.5f, 0.5f));
  myBsplineSurface.Setcontrolpoint(4, 5, vec3(1.5f,-0.5f, 1.0f));
  myBsplineSurface.Setcontrolpoint(4, 6, vec3(1.0f,-0.5f, 1.5f));

  myBsplineSurface.Setcontrolpoint(5, 0, vec3(0.5f,-1.0f,-1.5f));
  myBsplineSurface.Setcontrolpoint(5, 1, vec3(1.0f,-1.0f,-1.0f));
  myBsplineSurface.Setcontrolpoint(5, 2, vec3(1.5f,-1.0f,-0.5f));
  myBsplineSurface.Setcontrolpoint(5, 3, vec3(2.0f,-1.0f, 0.0f));
  myBsplineSurface.Setcontrolpoint(5, 4, vec3(1.5f,-1.0f, 0.5f));
  myBsplineSurface.Setcontrolpoint(5, 5, vec3(1.0f,-1.0f, 1.0f));
  myBsplineSurface.Setcontrolpoint(5, 6, vec3(0.5f,-1.0f, 1.5f));

  myBsplineSurface.Setcontrolpoint(6, 0, vec3(0.0f,-1.5f,-1.5f));
  myBsplineSurface.Setcontrolpoint(6, 1, vec3(0.5f,-1.5f,-1.0f));
  myBsplineSurface.Setcontrolpoint(6, 2, vec3(1.0f,-1.5f,-0.5f));
  myBsplineSurface.Setcontrolpoint(6, 3, vec3(1.5f,-1.5f, 0.0f));
  myBsplineSurface.Setcontrolpoint(6, 4, vec3(1.0f,-1.5f, 0.5f));
  myBsplineSurface.Setcontrolpoint(6, 5, vec3(0.5f,-1.5f, 1.0f));
  myBsplineSurface.Setcontrolpoint(6, 6, vec3(0.0f,-1.5f, 1.5f));

  Object bsplineObject = myBsplineSurface.generateObject();
  bsplineObject.init();
  BsplineSurface originbsplineSurface(myBsplineSurface);
  std::vector<Controlpoint> bsplineControlpoints;
  for (int i = 0; i < myBsplineSurface.control_points_m_.size(); i++)
  {
    for (int j = 0; j < myBsplineSurface.control_points_m_[i].size(); j++)
    {
      bsplineControlpoints.push_back(myBsplineSurface.control_points_m_[i][j]);
    }
  }

  GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    std::vector<GLfloat> lightvertexData;
    for (int i = 0; i < sizeof(vertices)/sizeof(GLfloat); i++)
    {
        lightvertexData.push_back(vertices[i]);
    }
    light.loadVertices(lightvertexData);
    light.InitLight();
    ///controlpoints init
    for (int i = 0; i < a_controlpoints.size(); i++)
    {
        a_controlpoints[i].loadVertices(lightvertexData);
        a_controlpoints[i].Init();
    }
    for (int i = 0; i < b_controlpoints.size(); i++)
    {
        b_controlpoints[i].loadVertices(lightvertexData);
        b_controlpoints[i].Init();
    }
    for (int i = 0; i < c_controlpoints.size(); i++)
    {
        c_controlpoints[i].loadVertices(lightvertexData);
        c_controlpoints[i].Init();
    }
    for (int i = 0; i < bsplineControlpoints.size(); i++)
    {
        bsplineControlpoints[i].loadVertices(lightvertexData);
        bsplineControlpoints[i].Init();
    }
    ///load shader
    Shader shader(getPath("shaders/vertex.vs"),getPath("shaders/fragment.fs"));
    Shader lightshader(getPath("shaders/lightvertex.vs"), getPath("shaders/lightfragment.fs"));

    ///init some parameters
    float lastChangetime = 0.0f;
    int state = DrawBezier;
    bool ifchange_ = false;
    ///Init imgui
    ImGui::CreateContext();     // Setup Dear ImGui context
    ImGui::StyleColorsDark();       // Setup Dear ImGui style
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");

  while (!glfwWindowShouldClose(window)) {

    float currentFrame = glfwGetTime();
    processInput(window);
    processDrawState(window, lastChangetime, currentFrame, state);

    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
    


    ///draw object
    shader.use();

    mat4 view = mycamera.getViewMatrix();
    mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH/(float)HEIGHT, 0.1f, 100.0f);
    
    
    ///set object's vertex shader
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    mat4 model = mat4(1.0f);
    shader.setMat4("model", model);
    
    ///set object's vertex shader
    shader.setVec3("objectColor", vec3(1.0f, 0.5f, 0.3f));
    shader.setVec3("lightColor", vec3(1.0f));///white light
    shader.setVec3("viewPos", mycamera.Position);
    shader.setVec3("lightPos", light.pos());
    
    if (state == DrawBspline)
    {
        showImgui(myBsplineSurface.control_points_m_, bsplineControlpoints, originbsplineSurface.control_points_m_,  ifchange_);
        if (ifchange_)
        {
            bsplineObject = myBsplineSurface.generateObject();
            bsplineObject.init();
            ifchange_ = false;
        }
        bsplineObject.drawElements();
        ///draw control points
        drawControlPoints(bsplineControlpoints, lightshader, view, projection);
    }
    else if (state == DrawBezier)
    {
        a_object.drawElements();
        shader.setVec3("objectColor", vec3(1.0f, 0.0f, 0.0f));
        b_object.drawElements();

        shader.setVec3("objectColor", vec3(0.0f, 1.0f, 0.0f));
        c_object.drawElements();
        ///draw control points
        drawControlPoints(a_controlpoints, lightshader, view, projection);
        drawControlPoints(b_controlpoints, lightshader, view, projection);
        drawControlPoints(c_controlpoints, lightshader, view, projection);
    }
    
    ///Use light's shader
    lightshader.use();

    ///set light's vertex shader
    lightshader.setMat4("view", view);
    lightshader.setMat4("projection", projection);
    model = mat4(1.0f);
    model = glm::translate(model, light.pos());
    model = glm::scale(model, vec3(0.2f));
    lightshader.setMat4("model", model);

    ///draw light
    light.Draw();

    


    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  ///clean up
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  light.Terminate();
  glfwTerminate();
  return 0;
}

void processInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    mycamera.processWalkAround(Forward);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    mycamera.processWalkAround(Backward);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    mycamera.processWalkAround(Leftward);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    mycamera.processWalkAround(Rightward);
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    std::cout << "Camera position: (" << mycamera.Position.x << ", "
         << mycamera.Position.y << ", " << mycamera.Position.z << ")" << std::endl;
  }
}


void processDrawState(GLFWwindow* window, float& lastChangetime, float currentFrame, int& state)
{
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && currentFrame - lastChangetime > 0.2f)
    {
        lastChangetime = currentFrame;
        if (state == DrawBezier)
            state = DrawBspline;
        else if (state == DrawBspline)
            state = DrawBezier;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        ifRightclicked = true;
    }
    else
    {
        ifRightclicked = false;
    }
}

void mouse_callback(GLFWwindow* window, double x, double y) {
    x = (float)x;
    y = (float)y;
    if (firstMouse) {
        lastX = x;
        lastY = y;
        firstMouse = false;
    }
    if (ifRightclicked)
        mycamera.processLookAround(x - lastX, lastY - y);

    // update record
    lastX = x;
    lastY = y;
}

void drawControlPoints(std::vector<Controlpoint> & controlpoints, Shader & shader, mat4 view, mat4 projection)
{
  shader.use();
  shader.setMat4("view", view);
  shader.setMat4("projection", projection);
  for (int i = 0; i < controlpoints.size(); i++)
  {
    mat4 model = mat4(1.0f);
    model = glm::translate(model, controlpoints[i].position_);
    model = glm::scale(model, vec3(0.01f));
    shader.setMat4("model", model);
    controlpoints[i].Draw();
  }
}

void showImgui(std::vector<std::vector<vec3>>& controlpoints, std::vector<Controlpoint>& drawpoints, std::vector<std::vector<vec3>>& originpoints, bool & ifchange_)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {
        static int i = 0;
        static int j = 0;
        static float x = controlpoints[i][j].x;
        static float y = controlpoints[i][j].y;
        static float z = controlpoints[i][j].z;

        static float default_x = originpoints[i][j].x;
        static float default_y = originpoints[i][j].y;
        static float default_z = originpoints[i][j].z;
        ImGui::Begin("Bspline parameters");
        if (ImGui::InputInt("the ith row", &i) || ImGui::InputInt("the jth column", &j))
        {
            x = controlpoints[i][j].x;
            y = controlpoints[i][j].y;
            z = controlpoints[i][j].z;
            default_x = originpoints[i][j].x;
            default_y = originpoints[i][j].y;
            default_z = originpoints[i][j].z;
        }
        
        ImGui::SliderFloat("X", &x, default_x - 0.3f, default_x + 0.3f);
        ImGui::SliderFloat("Y", &y, default_y - 0.3f, default_y + 0.3f);
        ImGui::SliderFloat("Z", &z, default_z - 0.3f, default_z + 0.3f);

        if (ImGui::Button("Set"))
        {
            controlpoints[i][j] = vec3(x, y, z);
            drawpoints[i * controlpoints.size() + j].position_ = controlpoints[i][j];
            ifchange_ = true;
        }
        ImGui::End();
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
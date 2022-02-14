#include <utils.h>
#include <light.h>
#include <camera.h>
#include <mesh.h>


const int WIDTH = 800;
const int HEIGHT = 600;

const int BUNNY = 0;
const int SPHERE = 1;
const int PLANE = 2;

GLFWwindow *window;

///Create a Camera
Camera camera(vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = (float)WIDTH/2, lastY = (float)HEIGHT/2;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

void CameraMovement();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void KeyBoardMeshChange(Mesh& mesh, bool& _ifchangeMesh, int& meshState, GLfloat currentFrame, GLfloat& lastChangeTime);

///Create  Lights
DirLight dirLight(vec3(-0.2f, -1.0f, -0.3f), vec3(0.05f), vec3(0.4f), vec3(0.5f));
Light light(vec3(3.2f, 3.0f, 0.0f), vec3(0.05f), vec3(0.8f), vec3(1.0f));
Light light2(vec3(2.2f, -2.0f, 2.0f), vec3(0.05f), vec3(0.35f, 0.5f, 0.0f), vec3(1.0f));
Light light3(vec3(2.2f, -2.0f, -2.0f), vec3(0.05f), vec3(0.35f, 0.0f, 0.35f), vec3(1.0f));
int main() {
    WindowGuard windowGuard(window, WIDTH, HEIGHT, "CS171 hw1");

    ///Set callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    ///disable cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glEnable(GL_DEPTH_TEST);

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
    ///init dataload process
    bool _ifchangeMesh = true;
    int meshState = BUNNY;
    GLfloat lastChangeTime = 0.0f;
    Mesh mesh;

    ///init the light
    std::vector<GLfloat> lightvertexData;
    for (int i = 0; i < sizeof(vertices)/sizeof(GLfloat); i++)
    {
        lightvertexData.push_back(vertices[i]);
    }
    light.loadVertices(lightvertexData);
    light.InitLight();
    light2.loadVertices(lightvertexData);
    light2.InitLight();
    light3.loadVertices(lightvertexData);
    light3.InitLight();
    /*
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    ///Bind VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    ///Bind VAO
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    */
    ///Shader
    //Shader shader(getPath("shaders/vertex.vs"),getPath("shaders/fragment.fs"));
    Shader shader(getPath("shaders/vertex.vs"), getPath("shaders/multifrag.fs"));
    Shader lightshader(getPath("shaders/lightvertex.vs"), getPath("shaders/lightfragment.fs"));

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);


 

        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        ///Move Camera
        glfwPollEvents();
        CameraMovement();

        /// <summary>
         /// Change mesh
         /// </summary>
         /// <returns></returns>
        glfwPollEvents();
        KeyBoardMeshChange(mesh, _ifchangeMesh, meshState, currentFrame, lastChangeTime);

        /// <summary>
        /// use object's shader
        /// </summary>
        /// <returns></returns>
        shader.use();

        mat4 view = camera.GetViewMatrix();
        mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH/(float)HEIGHT, 0.1f, 100.0f);

        ///set object's vertex shader
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        mat4 model = mat4(1.0f);
        shader.setMat4("model", model);

        ///set object's fragment shader
        shader.setVec3("material.ambient", vec3(1.0f, 0.5f, 0.31f));
        shader.setVec3("material.diffuse", vec3(1.0f, 0.5f, 0.31f));
        shader.setVec3("material.specular", vec3(0.5f));
        shader.setFloat("material.shininess", 32.0f);
        //shader.setVec3("objectColor", vec3(1.0f, 0.5f, 0.3f));
        
        ///Direction Light
        shader.setVec3("dirLight.direction", dirLight.Direction);
        shader.setVec3("dirLight.ambient", dirLight.Ambient);
        shader.setVec3("dirLight.diffuse", dirLight.Diffuse);
        shader.setVec3("dirLight.specular", dirLight.Specular);
        /// <summary>
        /// pointLights
        /// </summary>
        /// <returns></returns>
        shader.setVec3("pointLights[0].position", light.pos());
        shader.setVec3("pointLights[0].ambient", light.Ambient);
        shader.setVec3("pointLights[0].diffuse", light.Diffuse);
        shader.setVec3("pointLights[0].specular", light.Specular);
        shader.setFloat("pointLights[0].constant", light.Constant);
        shader.setFloat("pointLights[0].linear", light.Linear);
        shader.setFloat("pointLights[0].quadratic", light.Quadratic);

        shader.setVec3("pointLights[1].position", light2.pos());
        shader.setVec3("pointLights[1].ambient", light2.Ambient);
        shader.setVec3("pointLights[1].diffuse", light2.Diffuse);
        shader.setVec3("pointLights[1].specular", light2.Specular);
        shader.setFloat("pointLights[1].constant", light2.Constant);
        shader.setFloat("pointLights[1].linear", light2.Linear);
        shader.setFloat("pointLights[1].quadratic", light2.Quadratic);

        shader.setVec3("pointLights[2].position", light3.pos());
        shader.setVec3("pointLights[2].ambient", light3.Ambient);
        shader.setVec3("pointLights[2].diffuse", light3.Diffuse);
        shader.setVec3("pointLights[2].specular", light3.Specular);
        shader.setFloat("pointLights[2].constant", light3.Constant);
        shader.setFloat("pointLights[2].linear", light3.Linear);
        shader.setFloat("pointLights[2].quadratic", light3.Quadratic);
        
        //shader.setVec3("lightColor", vec3(1.0f));///white light
        shader.setVec3("viewPos", camera.Position);
        //shader.setVec3("lightPos", light.pos());
        /// <summary>
        /// draw mesh
        /// </summary>
        /// <returns></returns>
        mesh.draw();


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

        model = mat4(1.0f);
        model = glm::translate(model, light2.pos());
        model = glm::scale(model, vec3(0.2f));
        lightshader.setMat4("model", model);

        ///draw light
        light2.Draw();

        model = mat4(1.0f);
        model = glm::translate(model, light3.pos());
        model = glm::scale(model, vec3(0.2f));
        lightshader.setMat4("model", model);

        ///draw light
        light3.Draw();
        /*
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        */
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    mesh.Terminate();
    glfwTerminate();
    return 0;
}

void CameraMovement()
{
    if (keys[GLFW_KEY_W])
        camera.ProcessKeyBoard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S])
        camera.ProcessKeyBoard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A])
        camera.ProcessKeyBoard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D])
        camera.ProcessKeyBoard(RIGHT, deltaTime);
}

void KeyBoardMeshChange(Mesh & mesh, bool & _ifchangeMesh, int & meshState, GLfloat currentFrame, GLfloat& lastChangeTime)
{
    if (_ifchangeMesh == true)
    {
        if (meshState == BUNNY)
            mesh = Mesh(getPath("assets/bunny.object"));
        else if (meshState == SPHERE)
            mesh = Mesh(getPath("assets/sphere.object"));
        else if (meshState == PLANE)
            mesh = Mesh(getPath("assets/plane.object"));
        mesh.InitMesh();
        _ifchangeMesh = false;
    }

    if (keys[GLFW_KEY_X] && (currentFrame - lastChangeTime)>0.2f)
    {
        /// <summary>
        ///  Press X to change mesh
        /// </summary>
        /// <returns></returns>
        _ifchangeMesh = true;
        meshState = (meshState + 1) % 3;
        lastChangeTime = currentFrame;
    }
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos; // reversed y-coordinate from bottom

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

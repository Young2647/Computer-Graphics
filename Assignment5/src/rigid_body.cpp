#include <rigid_body.h>

///////////////////// RigidBody /////////////////////
glm::vec3 RigidBody::GetX()
{
    return this->x;
}

glm::vec3 RigidBody::GetV()
{
    return this->v;
}

float RigidBody::GetM()
{
    return this->M;
}

void RigidBody::Draw(Shader &shader, ShaderParam &shader_param)
{
    this->opengl_object.Draw(shader, shader_param);
}

void RigidBody::ClearTransientForces()
{
    this->transient_forces.clear();
}

void RigidBody::ClearSpeedTooSmall()
{
    if (abs(this->v.x) < MIN_SPEED) this->v.x = 0;
    if (abs(this->v.y) < MIN_SPEED) this->v.y = 0;
    if (abs(this->v.z) < MIN_SPEED) this->v.z = 0;

}
///////////////////// Sphere /////////////////////
Sphere::Sphere(
    glm::vec3 color,
    glm::vec3 x,
    glm::vec3 v,
    glm::vec3 w,
    float M,
    float radius)
{
    this->x = x;
    this->v = v;
    this->w = w;
    this->M = M;

    this->radius = radius;

    InitOpenGLObject(color);
    InitInertiaTensor();

    Force G(glm::vec3(0.0f, 0.0f, 0.0f), this->M * g);
    this->constant_forces.push_back(G);

    this->P = this->M * this->v;
    this->L = this->I * this->w;
}

/**
 * @brief Get the radius of the sphere.
 * 
 * @return float 
 */
float Sphere::GetRadius()
{
    return this->radius;
}

/**
 * @brief This function will update the sphere's state forward for the time step dt.
 * 
 * @param[in] dt 
 */
void Sphere::Forward(float dt)
{
    for (Force const_force : constant_forces)
    {
        this->P += const_force.F * dt;
    }
    for (Force trans_force : transient_forces)
    {
        this->P += trans_force.F * dt;
    }
    this->v = this->P / this->M;
    //ClearSpeedTooSmall();
    this->x += this->v * dt;
    UpdateOpenGLObject();
    /// TODO: Your code here.
}

void Sphere::Forward(float dt , bool only_position)
{
    this->x += this->v * dt;
    UpdateOpenGLObject();
    /// TODO: Your code here.
}

/**
 * @brief This function will update the sphere's state backward for the time step dt.
 *        You may need this function to handle the collision.
 * 
 * @param[in] dt 
 */
void Sphere::Backward(float dt)
{
    this->x -= this->v * dt;

    /// TODO: Your code here.
    for (Force const_force : constant_forces)
    {
        this->P -= const_force.F * dt;
    }
    for (Force trans_force : transient_forces)
    {
        this->P -= trans_force.F * dt;
    }
    this->v = this->P / M;
     UpdateOpenGLObject();
}

void Sphere::Backward(float dt, bool onlyposition)
{
    this->x -= this->v * dt;
    UpdateOpenGLObject();
}
/**
 * @brief If you consider the rotation as well, you need to handle the intertia tensor.
 * 
 */
void Sphere::InitInertiaTensor()
{
    return;
}

/**
 * @brief This function initializes the opengl object for rendering.
 *        Specifically, the vertices, indices are generated.
 * 
 * @param[in] color 
 */
void Sphere::InitOpenGLObject(glm::vec3 color)
{
    opengl_object.color = color;

    float delta = 0.01;
    int size = 2 * PI / delta + 1;

    opengl_object.vertices.resize(size * size);
    for (int t = 0; t < size; t++)
    {
        float theta = (t == size - 1) ? (2 * PI) : t * delta;
        for (int p = 0; p < size; p++)
        {
            float phi = (p == size - 1) ? (2 * PI) : p * delta;

            float x = radius * cosf(phi) * sinf(theta);
            float y = radius * sinf(phi) * sinf(theta);
            float z = radius * cosf(theta);

            opengl_object.vertices[t * size + p].position = glm::vec3(x, y, z);
            opengl_object.vertices[t * size + p].normal = normalize(glm::vec3(x, y, z));
        }
    }

    opengl_object.indices.reserve(3 * 2 * (size - 1) * (size - 1));
    for (int t = 0; t < size - 1; t++)
    {
        for (int p = 0; p < size - 1; p++)
        {
            opengl_object.indices.push_back(t * size + p);
            opengl_object.indices.push_back(t * size + p + 1);
            opengl_object.indices.push_back((t + 1) * size + p);

            opengl_object.indices.push_back(t * size + p + 1);
            opengl_object.indices.push_back((t + 1) * size + p);
            opengl_object.indices.push_back((t + 1) * size + p + 1);
        }
    }

    opengl_object.M = glm::translate(glm::mat4(1.0f), this->x);

    opengl_object.InitData();
}

/**
 * @brief This functioin updates the opengl object for rendering.
 *        We do not consider the rotation so we only need to handle the translation.
 *        
 */
void Sphere::UpdateOpenGLObject()
{
    opengl_object.M = glm::translate(glm::mat4(1.0f), this->x);
}

///////////////////// Wall /////////////////////
Wall::Wall(
    glm::vec3 color,
    glm::vec3 x,
    glm::vec3 s1,
    glm::vec3 s2)
{
    this->x = x;
    this->s1 = s1;
    this->s2 = s2;

    this->v = glm::vec3(0.0f);
    this->w = glm::vec3(0.0f);
    this->M = INFINITY;

    InitInertiaTensor();
    InitOpenGLObject(color);

    this->P = glm::vec3(0.0f);
    this->L = glm::vec3(0.0f);
}

glm::vec3 Wall::GetS1()
{
    return this->s1;
}

glm::vec3 Wall::GetS2()
{
    return this->s2;
}

void Wall::Forward(float dt)
{
    return;
}

void Wall::Backward(float dt)
{
    return;
}

void Wall::InitInertiaTensor()
{
    return;
}

void Wall::InitOpenGLObject(glm::vec3 color)
{
    opengl_object.color = color;

    opengl_object.vertices.resize(4);
    opengl_object.vertices[0].position = this->x - s1 / 2.0f + s2 / 2.0f; // top-left
    opengl_object.vertices[1].position = this->x + s1 / 2.0f + s2 / 2.0f; // top-right
    opengl_object.vertices[2].position = this->x - s1 / 2.0f - s2 / 2.0f; // bottom-left
    opengl_object.vertices[3].position = this->x + s1 / 2.0f - s2 / 2.0f; // bottom-right
    opengl_object.vertices[0].normal = normalize(cross(s1, s2));
    opengl_object.vertices[1].normal = opengl_object.vertices[0].normal;
    opengl_object.vertices[2].normal = opengl_object.vertices[0].normal;
    opengl_object.vertices[3].normal = opengl_object.vertices[0].normal;

    opengl_object.indices = {0, 1, 2, 1, 2, 3};

    opengl_object.M = glm::mat4(1.0f);

    opengl_object.InitData();

    return;
}

void Wall::UpdateOpenGLObject()
{
    return;
}

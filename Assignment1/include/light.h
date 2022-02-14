#ifndef _LIGHT_H_
#define _LIGHT_H_
#include "defines.h"
#include <vector>

const vec3 AMBIENT = vec3(0.05f);
const vec3 DIFFUSE = vec3(0.5f);
const vec3 SPECULAR = vec3(1.0f);
const float CONSTANT = 1.0f;
const float LINEAR = 0.09f;
const float QUADRATIC = 0.032f;

class Light
{
public :
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
    float Constant;
    float Linear;
    float Quadratic;
    Light(vec3 position);
    Light(vec3 position, vec3 ambient, vec3 diffuse, vec3 specular);
    void Draw();
    vec3 pos();
    void InitLight();
    void Terminate();
    void loadVertices(std::vector<GLfloat> & vertexData);
    private :

    vec3 Position;
    std::vector<GLfloat> vertices;
    unsigned int VAO, VBO;
};

class DirLight
{
public :
    vec3 Direction;
    
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
    DirLight(vec3 direction);
    DirLight(vec3 direction, vec3 ambient, vec3 diffuse, vec3 specular);
};

#endif
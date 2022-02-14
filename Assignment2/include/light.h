#ifndef _LIGHT_H_
#define _LIGHT_H_
#include "defines.h"
#include <vector>
class Light
{
    public :
    Light(vec3 position);
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


#endif
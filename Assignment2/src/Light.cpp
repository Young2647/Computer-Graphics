#include <light.h>

Light::Light(vec3 position)
{
    Position = position;
}

vec3
Light::pos()
{
    return Position;
}


void
Light::loadVertices(std::vector<GLfloat> & vertexData)
{
    vertices = vertexData;
}

void
Light::InitLight()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    ///Bind VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
    
    ///Bind VAO
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void
Light::Draw()
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void
Light::Terminate()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}
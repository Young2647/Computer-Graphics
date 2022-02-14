#include <opengl_object.h>

OpenGLObject::OpenGLObject()
{
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
}

void OpenGLObject::InitData()
{
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

    // bind buffers

    // configue vertex attributes
    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    glEnableVertexAttribArray(1); // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));

    // unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void OpenGLObject::Draw(Shader& shader, ShaderParam& shader_param)
{
    shader.use();
    shader.setMat4("model", M);
    shader.setMat4("view", shader_param.view);
    shader.setMat4("projection", shader_param.projection);
    shader.setVec3("cameraPos", shader_param.cameraPos);
    shader.setVec3("objectColor", color);

    glBindVertexArray(VAO);
    if (draw_option.draw_element)
    {
        glDrawElements(draw_option.primitive_mode, indices.size(), GL_UNSIGNED_INT, 0);
    }
    else
    {
         glDrawArrays(draw_option.primitive_mode, 0, vertices.size());
    }

    glBindVertexArray(0);
}
#version 330 core

in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform vec3 cameraPos;

void main()
{
    color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}   
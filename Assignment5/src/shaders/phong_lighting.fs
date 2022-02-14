#version 330 core

in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform vec3 cameraPos;
uniform vec3 objectColor;

void main()
{
    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
    vec3 lightPos = vec3(0.0f, 9.0f, 0.0f);

    // ambient
    float ambientStrength = 0.3f;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    float diffuseStrength = 0.8f;
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(lightPos-FragPos);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diffuseStrength * diff * lightColor;

    // specular
    float specularStrength = 0.2f;
    vec3 cameraDirection = normalize(cameraPos - FragPos);
    vec3 reflectDirection = reflect(-lightDirection, norm);  
    float spec = pow(max(dot(cameraDirection, reflectDirection), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  

    vec3 tmp = (ambient + diffuse + specular);
    color = vec4(tmp, 1.0f) * vec4(objectColor, 1.0f);
}   
#version 330 core
out vec4 color;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

vec3 Phonglighting(vec3 _normal){
    //Ambient
    float ambientStrength = 0.35f;
    vec3 ambient = ambientStrength * lightColor;

    //Diffuse
    vec3 norm = normalize(_normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor;

    //Specular
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular =specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    return result;
}

void main()
{
    if(gl_FrontFacing){
        color = vec4(Phonglighting(Normal),1.0);
    }
    else{
        color = vec4(Phonglighting(-Normal),1.0);
    }
    
}
#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

void main()
{
    vec3 color = texture(diffuseMap, TexCoords).rgb;
    vec3 normal = texture(normalMap, TexCoords).rgb;
    normal = normalize(2.0 * normal - 1.0); // µ¥Î»»¯

    // ambient
    vec3 ambient = 0.1 * color;

    // diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * color;

    // specular
    // Blinn-Phong
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = spec * color;

    FragColor = vec4((ambient + diffuse + specular), 1.0);
}
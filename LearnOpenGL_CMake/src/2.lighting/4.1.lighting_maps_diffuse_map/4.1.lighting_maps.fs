#version 330 core

struct Material {
    sampler2D diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

//uniform sampler2D texture1;
uniform Material material;
uniform Light light;

uniform vec3 viewPos;


out vec4 FragColor;

void main()
{
    //ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

    //diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0); //获取cos值表示模型法线和光漫反射夹角的影响
    vec3 diffuse = diff * light.diffuse * texture(material.diffuse, TexCoords).rgb;

    //specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm); //光线对物体的反射向量
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = material.specular * spec * light.specular;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
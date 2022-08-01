#version 330 core

//设置材质
struct Material {
	sampler2D diffuse;//漫反射贴图
	sampler2D specular;
	float shininess;
};

struct Light {
	vec3 position;
	vec3 direction;
	float cutOff;//内切光角
	float outerCutOff;//外切光角

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

uniform Material material;
uniform Light light;

uniform vec3 viewPos;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 color;



void main()
{
	//环境光
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

	//漫反射
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

	//镜面光
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

	//光照衰减
	float distance = length(light.position - FragPos);
	float attenuation = 1.0f / (light.constant + light.linear * distance +
		light.quadratic * distance * distance);

	float theta = dot(lightDir, normalize(-light.direction));
	if (theta > light.cutOff) {
		vec3 result = (ambient + diffuse + specular) * attenuation;
		color = vec4(result, 1.0f);
	}
	else {
		//边缘平滑处理
		float epsilon = light.cutOff - light.outerCutOff;
		float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f);
		diffuse *= intensity;
		specular *= intensity;
		vec3 result = (ambient + diffuse + specular) * attenuation;
		color = vec4(result, 1.0f);
	}
}

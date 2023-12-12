#version 330 core

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;

//…Ë÷√π‚’’ Ù–‘
struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform Light light;

in vec2 TexCoords;

out vec4 color;



void main()
{
	color = texture(texture_diffuse1, TexCoords);
}

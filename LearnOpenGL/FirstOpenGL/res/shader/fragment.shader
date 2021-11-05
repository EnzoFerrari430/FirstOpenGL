#version 330 core


in vec2 v_TexCoord;
out vec4 color;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
	vec4 textColor = mix(texture(texture1, v_TexCoord), texture(texture2, v_TexCoord), 0.2);
	color = textColor;
}

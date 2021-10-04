#shader vertex
#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord;

out vec2 v_TexCoord;

uniform mat4 u_MVP;
uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;

void main()
{
	//gl_Position = u_Proj * u_View * u_Model * vec4(position.x, position.y, 0.0, 1.0);
	gl_Position = u_MVP * vec4(position.x, position.y, 0.0, 1.0);
	v_TexCoord = texCoord;
};

#shader fragment
#version 330 core
in vec2 v_TexCoord;

out vec4 color;

uniform vec4 u_Color;
uniform sampler2D u_Texture;

void main()
{
	vec4 texColor = texture(u_Texture, v_TexCoord) * u_Color;
	color = texColor;
};
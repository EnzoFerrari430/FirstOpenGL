#version 330 core

layout(location=0) in vec3 aPosition;
layout(location=1) in vec2 aTexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 FragPos;
out vec2 TexCoords;

void main()
{
    FragPos = vec3(model * vec4(aPosition, 1.0));
    TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPosition, 1.0);
}
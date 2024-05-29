#version 460 core

layout(location=0) in vec3 aPosition;
layout(location=2) in vec2 aTexCoords;

layout(std140, binding=0) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPosition, 1.0);
}
#version 460 core

layout(location=0) in vec3 aPosition;
layout(location=2) in vec2 aTexCoords;
layout(location=3) in mat4 aTransMatrix;

layout(std140, binding=0) uniform Matrices
{
    mat4 projection;    // 16 16 16 16
    mat4 view;          // 16 16 16 16
};

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = projection * view * aTransMatrix * vec4(aPosition, 1.0);
}
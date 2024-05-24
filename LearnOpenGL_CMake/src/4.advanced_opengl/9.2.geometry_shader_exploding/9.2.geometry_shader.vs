#version 330 core

layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out VS_OUT
{
    vec2 texCoords;
} vs_out;

void main()
{
    vs_out.texCoords = aTexCoords;

    gl_Position = projection * view * model * vec4(aPosition, 1.0);
}
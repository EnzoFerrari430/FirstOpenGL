#version 330 core

layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;


void main()
{
    vs_out.FragPos = vec3(model * vec4(aPosition, 1.0));
    vs_out.Normal = mat3(transpose(inverse(model))) * aNormal; // inverse反向缩放 transpose反向旋转
    vs_out.TexCoords = aTexCoords;

    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
}
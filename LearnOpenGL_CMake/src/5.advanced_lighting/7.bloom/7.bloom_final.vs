#version 330 core

layout(location=0) in vec3 aPosition;
layout(location=1) in vec2 aTexCoords;

out VS_OUT
{
    vec2 TexCoords;
} vs_out;



void main()
{
    vs_out.TexCoords = aTexCoords;
    gl_Position = vec4(aPosition, 1.0);
}
#version 450 core

layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoords;

// ½Ó¿Ú¿é
out VS_OUT
{
    vec3 Normal;
    vec3 Position;
    vec2 TexCoords;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    vs_out.TexCoords = aTexCoords;
    vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;
    vec4 modelPos = model * vec4(aPosition, 1.0);
    vs_out.Position = vec3(modelPos);
    gl_Position = projection * view * modelPos;
    //gl_PointSize = gl_Position.z;
}
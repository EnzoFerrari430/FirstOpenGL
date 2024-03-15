#version 330 core

layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aNormal;

out vec3 Normal;
out vec3 Position;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    Normal = mat3(transpose(inverse(model))) * aNormal;
    vec4 modelPos = model * vec4(aPosition, 1.0);
    Position = vec3(modelPos);
    gl_Position = projection * view * modelPos;
}
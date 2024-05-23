#version 450 core

layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoords;

out vec3 Normal;
out vec3 Position;
out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    TexCoords = aTexCoords;
    Normal = mat3(transpose(inverse(model))) * aNormal;
    vec4 modelPos = model * vec4(aPosition, 1.0);
    Position = vec3(modelPos);
    gl_Position = projection * view * modelPos;
    //gl_PointSize = gl_Position.z;
}
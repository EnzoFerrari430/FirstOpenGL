#version 450 core

layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoords;

// 140²¼¾Ö  uniform binding pointÎª0
layout(std140, binding=0) uniform Matrices
{
    mat4 projection;    //16 16 16 16
    mat4 view;          //16 16 16 16
    mat4 model;         //16 16 16 16
};

out VS_OUT
{
    vec3 normal;
    mat4 projection;
} vs_out;


void main()
{
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    vs_out.normal = vec3(vec4(normalMatrix * aNormal, 0.0));
    vs_out.projection = projection;

    gl_Position = view * model * vec4(aPosition, 1.0);

}
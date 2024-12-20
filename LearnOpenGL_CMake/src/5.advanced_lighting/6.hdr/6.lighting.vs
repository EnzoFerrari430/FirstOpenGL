#version 330 core

layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform bool inverse_normal;

out VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPosition, 1.0));
    vs_out.TexCoords = aTexCoords;

    vec3 n = inverse_normal ? -aNormal : aNormal;
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vs_out.Normal = normalize(normalMatrix * n);

    gl_Position = projection * view * model * vec4(aPosition, 1.0f);
}
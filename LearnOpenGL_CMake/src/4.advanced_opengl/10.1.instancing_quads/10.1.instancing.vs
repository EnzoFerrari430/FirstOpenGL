#version 330 core
layout(location=0) in vec2 aPosition;
layout(location=1) in vec3 aColor;

out vec3 fColor;

// 这里最大1024个实例
uniform vec2 offsets[1024];

void main()
{
    //在调用实例化渲染时glDrawArraysInstanced或glDrawElementsInstanced
    //gl_InstanceID会从0开始，在每个实例被渲染时递增1
    vec2 offset = offsets[gl_InstanceID];
    gl_Position = vec4(aPosition + offset, 0.0, 1.0);
    fColor = aColor;
}
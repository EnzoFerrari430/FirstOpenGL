#version 450 core

layout(location=0) in vec3 aPosition;

// layout(std140) 用于定义Uniform块对它的内容使用一个特定的内存布局
/*
每4个字节将会用一个N来表示
std140类型布局规则
标量                    比如int和bool 每个标量的基准对齐量为N。
向量                    2N或者4N。这意味着vec3的基准对齐量为4N。
标量或向量的数组         每个元素的基准对齐量与vec4的相同。
矩阵	储存为列向量的数组   每个向量的基准对齐量与vec4的相同。
结构体                  等于所有元素根据规则计算后的大小，但会填充到vec4大小的倍数。
*/
layout(std140, binding = 0) uniform Matrices
{
    mat4 projection;    //16 16 16 16
    mat4 view;          //16 16 16 16
};

uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(aPosition, 1.0);
}
#version 450 core

layout(location=0) in vec3 aPosition;

// layout(std140) ���ڶ���Uniform�����������ʹ��һ���ض����ڴ沼��
/*
ÿ4���ֽڽ�����һ��N����ʾ
std140���Ͳ��ֹ���
����                    ����int��bool ÿ�������Ļ�׼������ΪN��
����                    2N����4N������ζ��vec3�Ļ�׼������Ϊ4N��
����������������         ÿ��Ԫ�صĻ�׼��������vec4����ͬ��
����	����Ϊ������������   ÿ�������Ļ�׼��������vec4����ͬ��
�ṹ��                  ��������Ԫ�ظ��ݹ�������Ĵ�С��������䵽vec4��С�ı�����
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
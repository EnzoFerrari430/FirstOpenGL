#version 330 core
layout(location=0) in vec2 aPosition;
layout(location=1) in vec3 aColor;

out vec3 fColor;

// �������1024��ʵ��
uniform vec2 offsets[1024];

void main()
{
    //�ڵ���ʵ������ȾʱglDrawArraysInstanced��glDrawElementsInstanced
    //gl_InstanceID���0��ʼ����ÿ��ʵ������Ⱦʱ����1
    vec2 offset = offsets[gl_InstanceID];
    gl_Position = vec4(aPosition + offset, 0.0, 1.0);
    fColor = aColor;
}
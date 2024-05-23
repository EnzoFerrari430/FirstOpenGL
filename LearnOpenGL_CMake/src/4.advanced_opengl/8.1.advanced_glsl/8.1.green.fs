#version 450 core

out vec4 FragColor;
layout (depth_greater) out float gl_FragDepth;

// �ӿڿ��������Ҫ�붥����ɫ��������һ��
// �ӿڿ��ʵ������������, ������������ȷ��Ｔ��
in VS_OUT
{
    vec3 Normal;
    vec3 Position;
    vec2 TexCoords;
} fs_in;


uniform sampler2D frontTexture;
uniform sampler2D backTexture;

void main()
{
    if(gl_FrontFacing)
        FragColor = texture(frontTexture, fs_in.TexCoords);
    else
        FragColor = texture(backTexture, fs_in.TexCoords);

    gl_FragDepth = gl_FragCoord.z + 0.01;
}
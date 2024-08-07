#version 330 core

in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main()
{
    float lightDistance = length(FragPos.xyz - lightPos);

    // ӳ�䵽0-1֮��
    lightDistance = lightDistance / far_plane;

    gl_FragDepth = lightDistance;
}
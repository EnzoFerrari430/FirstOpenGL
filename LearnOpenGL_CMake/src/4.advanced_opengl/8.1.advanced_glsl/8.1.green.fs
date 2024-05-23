#version 450 core

out vec4 FragColor;
layout (depth_greater) out float gl_FragDepth;

in vec3 Normal;
in vec3 Position;
in vec2 TexCoords;

uniform sampler2D frontTexture;
uniform sampler2D backTexture;

void main()
{
    if(gl_FrontFacing)
        FragColor = texture(frontTexture, TexCoords);
    else
        FragColor = texture(backTexture, TexCoords);

    gl_FragDepth = gl_FragCoord.z + 0.01;
}
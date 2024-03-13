#version 330 core

out vec4 FragColor;

uniform sampler2D screenTexture;

in vec2 TexCoords;

void main()
{
    vec3 color = texture(screenTexture, TexCoords).rgb;
    FragColor = vec4(color, 1.0);
}
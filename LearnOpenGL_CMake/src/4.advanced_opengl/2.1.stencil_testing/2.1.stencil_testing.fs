#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, TexCoords);
    //FragColor = vec4(TexCoords.x, TexCoords.y, 1.0, 1.0);
}
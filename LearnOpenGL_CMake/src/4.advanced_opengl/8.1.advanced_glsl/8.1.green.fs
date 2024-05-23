#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{
    //FragColor = texture(texture1, TexCoords);
    FragColor = vec4(0.0, 1.0, 0.0, 1.0);
}
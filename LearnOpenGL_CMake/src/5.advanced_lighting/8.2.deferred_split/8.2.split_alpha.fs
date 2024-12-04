#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{
    vec3 color = texture(texture1, TexCoords).rgb;
    float alpha = texture(texture1, TexCoords).a;

    FragColor = vec4(alpha);
}

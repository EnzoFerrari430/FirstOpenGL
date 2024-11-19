#version 330 core

out vec4 FragColor;

in VS_OUT
{
    vec2 TexCoords;
} fs_in;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform bool bloom;
uniform float exposure;

void main()
{
    const float gamma = 2.2;
    vec3 hdrColor = texture(scene, fs_in.TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlur, fs_in.TexCoords).rgb;
    if(bloom)
        hdrColor += bloomColor;
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    result = pow(result, vec3(1.0/gamma));
    FragColor = vec4(result, 1.0);
}
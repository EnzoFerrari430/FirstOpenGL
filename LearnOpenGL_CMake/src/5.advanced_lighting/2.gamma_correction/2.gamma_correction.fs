#version 330 core

out vec4 FragColor;

in VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D floorTexture;

uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];
uniform vec3 viewPos;
uniform bool applyGamma;

vec3 BlinnPhong(vec3 normal, vec3 fragPos, vec3 lightPos, vec3 lightColor)
{

    // ambient
    vec3 ambient = 0.05 * lightColor;

    // diffuse
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    //float gamma = 2.2;
    //diffuse = pow(diffuse, vec3(gamma));

    // specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;

    // simple attenuation
    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (applyGamma ? distance * distance : distance);
    diffuse *= attenuation;
    specular *= attenuation;

    // 重校,对sRGB纹理进行颜色计算前变回线性空间
    // 应对,glEnable(GL_FRAMEBUFFER_SRGB) 启动sRGB的情况
    //float gamma = 2.2;
    //return pow(ambient + diffuse + specular, vec3(gamma));
    return ambient + diffuse + specular;
}

void main()
{
    vec3 color = texture(floorTexture, fs_in.TexCoords).rgb;
    vec3 lighting = vec3(0.0);
    for(int i = 0 ; i < 4 ; ++i)
    {
        lighting += BlinnPhong(fs_in.Normal, fs_in.FragPos, lightPositions[i], lightColors[i]);
    }
    color *= lighting;

    // apply gamma correction
    if(applyGamma)
    {
        float gamma = 2.2;
        color = pow(color, vec3(1.0 / gamma));
    }

    FragColor = vec4(color, 1.0);
}
#version 330 core

out vec4 FragColor;

in VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // 将NDC坐标xyz都变换到0-1范围
    // xy可以对应深度贴图的xy坐标，z可以对应深度贴图的深度值
    projCoords = projCoords * 0.5 + 0.5;

    // 获取光的位置视野下最近的深度
    float closestDepth = texture(shadowMap, projCoords.xy).r;

    // 当前片段在光的位置视野下的深度
    float currentDepth = projCoords.z;

    // 使用偏移量，采样点可以比表面深度值更小, 解决阴影失真问题
    // 但是会产生另一个问题 阴影悬浮
    float bias = 0.005;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

void main()
{
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(1.0);

    // Ambient
    vec3 ambient = 0.15 * color;
    // Diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // Specular  Blinn-Phong
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 halfWayDir = normalize(lightDir + viewDir);
    float spec = 0.0;
    spec = pow(max(dot(halfWayDir, normal), 0.0), 64.0);
    vec3 specular = spec * lightColor;

    // 计算阴影
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    FragColor = vec4(lighting, 1.0f);
}
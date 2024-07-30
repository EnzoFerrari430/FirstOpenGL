#version 330 core

out vec4 FragColor;

in VS_OUT
{
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform float heightScale;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);

void main()
{
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec2 texCoords = ParallaxMapping(fs_in.TexCoords, viewDir);

    // 丢弃超出边缘的部分
    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;

    vec3 color = texture(diffuseMap, texCoords).rgb;
    vec3 normal = texture(normalMap, texCoords).rgb;
    normal = (2.0 * normal - 1.0); // 单位化

    // ambient
    vec3 ambient = 0.1 * color;

    // diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * color;

    // specular
    // Blinn-Phong
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = spec * color;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    // 深度遍历
    const float minLayers = 8;
    const float maxLayers = 32;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
    float layerDepth = 1.0 / numLayers;

    float currentLayerDepth = 0.0;

    // 记录纹理坐标的总偏移量
    vec2 p = viewDir.xy * heightScale;
    vec2 deltaTexCoords = p / numLayers;

    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = texture(depthMap, currentTexCoords).r;
    while(currentLayerDepth < currentDepthMapValue)
    {
        // 每次偏移一个deltaTexCoords的量
        currentTexCoords -= deltaTexCoords;

        currentDepthMapValue = texture(depthMap, currentTexCoords).r;

        currentLayerDepth += layerDepth;
    }

    // get texture coordinates before collision
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(depthMap, prevTexCoords).r - currentLayerDepth + layerDepth;

    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoods = prevTexCoords * weight + currentTexCoords * (1 - weight);

    return finalTexCoods;
}
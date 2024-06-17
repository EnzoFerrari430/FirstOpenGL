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
    // ִ��͸�ӳ���
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // ��NDC����xyz���任��0-1��Χ
    // xy���Զ�Ӧ�����ͼ��xy���꣬z���Զ�Ӧ�����ͼ�����ֵ
    projCoords = projCoords * 0.5 + 0.5;

    // ���ó�������׶�����Ӱ�������Ϊ0
    if(projCoords.z > 1.0)
        return 0.0;

    // ��ǰƬ���ڹ��λ����Ұ�µ����
    float currentDepth = projCoords.z;

    //// ��ȡ���λ����Ұ����������
    //float closestDepth = texture(shadowMap, projCoords.xy).r;
    //
    //// ʹ��ƫ��������������Աȱ������ֵ��С, �����Ӱʧ������
    //// ���ǻ������һ������ ��Ӱ����
    float bias = 0.005;
    //float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    //PCF����������Ӱʱ���ǿ��ǵ��������㣬������һ����Χ�ڽ��ж��ز�����������������Ӱ�ı�Ե����ô���
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0); //����ʵ�ʴ�С��0-1����ӳ���ڵĴ�С
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;


    return shadow;
}

void main()
{
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.3);

    // Ambient
    vec3 ambient = 0.3 * color;
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

    // ������Ӱ
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    FragColor = vec4(lighting, 1.0f);
}
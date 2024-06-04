#version 330 core

out vec4 FragColor;

in VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D texture1;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool blinn;

void main()
{
    vec3 color = texture(texture1, fs_in.TexCoords).rgb;

    // ambient
    vec3 ambient = 0.05 * color;

    //diffuse
    vec3 norm = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(norm, lightDir), 0.0); //��ȡcosֵ��ʾģ�ͷ��ߺ͹�������нǵ�Ӱ��
    vec3 diffuse = diff * color;

    //specular
    //Blinn-Phong�������Ƿ�����������֮��ļнǣ�������ģ�Ͳ������ǹ۲췽���뷴��������ļнǡ�
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    float shininess1 = 8.0, shininess2 = 32.0;
    float spec = 0.0;
    if(blinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(norm, halfwayDir), 0.0), shininess2);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, norm); //���߶�����ķ�������
        spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess1);
    }
    vec3 specular = spec * color;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
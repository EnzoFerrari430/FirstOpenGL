//#version 330 core
//
//out vec4 FragColor;
//
//in vec2 TexCoords;
//
//uniform sampler2D screenTexture;
//
//void main()
//{
//    //vec3 color = texture(screenTexture, TexCoords).rgb;
//    //FragColor = vec4(color, 1.0);
//
//
//    /*
//    反相
//    vec3 color = vec3(1.0 - texture(screenTexture, TexCoords).rgb);
//    FragColor = vec4(color, 1.0);
//    */
//
//    /*
//    灰度
//    FragColor = texture(screenTexture, TexCoords);
//    //float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
//    float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
//    FragColor = vec4(average, average, average, 1.0);
//    */
//}

// 核(Kernel)（或卷积矩阵(Convolution Matrix)）是一个类矩阵的数值数组，
// 它的中心为当前的像素，它会用它的核值乘以周围的像素值，并将结果相加变成一个值。
// 所以，基本上我们是在对当前像素周围的纹理坐标添加一个小的偏移量，并根据核将结果合并。
// 比如有锐化核, 模糊核, 边缘检测核
#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

const float offset = 1.0 / 300.0;

void main()
{
    vec2 offsets[9] = vec2[](
        vec2(-offset, offset),
        vec2(0.0f,    offset),
        vec2(offset,  offset),
        vec2(-offset, 0.0f),
        vec2(0.0f,    0.0f),
        vec2(offset,  0.0f),
        vec2(-offset, -offset),
        vec2(0.0f,    -offset),
        vec2(offset,  -offset)
    );

    //// 锐化核 所有值的和为1
    //float kernel[9] = float[](
    //    -1, -1, -1,
    //    -1,  9, -1,
    //    -1, -1, -1
    //);

    //// 模糊核 所有值的和为1
    //float kernel[9] = float[](
    //    1.0 / 16, 2.0 / 16, 1.0 / 16,
    //    2.0 / 16, 4.0 / 16, 2.0 / 16,
    //    1.0 / 16, 2.0 / 16, 1.0 / 16
    //);

    //边缘检测核
    float kernel[9] = float[](
        1, 1, 1,
        1, -8, 1,
        1, 1, 1
    );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; ++i)
    {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }

    vec3 color = vec3(0.0);
    for(int i = 0; i < 9; ++i)
    {
        color += sampleTex[i] * kernel[i];
    }
    FragColor = vec4(color, 1.0);
}

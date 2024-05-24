#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 5) out;

//gs_in[]接口块被声明为一个数组，因为大多数的渲染图元包含多于1个的顶点，而几何着色器的输入是一个图元的所有顶点。
in VS_OUT
{
    vec3 color;
} gs_in[];

out vec3 fColor;

void build_house(vec4 position)
{
    fColor = gs_in[0].color;
    gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0); //左下
    EmitVertex();
    gl_Position = position + vec4(0.2, -0.2, 0.0, 0.0); //右下
    EmitVertex();
    gl_Position = position + vec4(-0.2, 0.2, 0.0, 0.0); //左上
    EmitVertex();
    gl_Position = position + vec4(0.2, 0.2, 0.0, 0.0); //右上
    EmitVertex();
    gl_Position = position + vec4(0.0, 0.4, 0.0, 0.0); //顶部
    fColor = vec3(1.0, 1.0, 1.0);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    build_house(gl_in[0].gl_Position);
}
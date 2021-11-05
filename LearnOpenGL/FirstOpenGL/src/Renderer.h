#pragma once

#include <glad/glad.h>
#include "VertexArray.h"
#include "Shader.h"
#include "IndexBuffer.h"


#define ASSERT(x) if (!(x)) __debugbreak()
//缺陷：要考虑作用域的问题
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

class Renderer
{
public:
	void setBGColor(float r, float g, float b, float a) const;
	void clear(int clearFlags) const;
	void draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
	void draw(const VertexArray& va, int first, int count, const Shader& shader) const;
};
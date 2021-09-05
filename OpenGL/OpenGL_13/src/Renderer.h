#pragma once

#include <GL/glew.h>

#define ASSERT(x) if (!(x)) __debugbreak()
//ȱ�ݣ�Ҫ���������������
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);
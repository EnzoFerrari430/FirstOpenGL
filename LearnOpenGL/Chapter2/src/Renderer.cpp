#include "Renderer.h"
#include <iostream>


void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
		//std::cout << 1 << std::endl;
}

bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" << error << "): " << function <<
			" " << file << ":" << line << std::endl;
		return false;
	}
	return true;
}

void Renderer::setBGColor(float r, float g, float b, float a) const
{
	glClearColor(r, g, b, a);
}

void Renderer::clear(int clearFlags) const
{
	glClear(clearFlags);
}

void Renderer::draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
	shader.bind();
	va.bind();
	ib.bind();

	GLCall(glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::draw(const VertexArray& va, int first, int count, const Shader& shader) const
{
	shader.bind();
	va.bind();
	GLCall(glDrawArrays(GL_TRIANGLES, first, count));
}
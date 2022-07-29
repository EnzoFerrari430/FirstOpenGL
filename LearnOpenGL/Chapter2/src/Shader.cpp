#include "Shader.h"
#include "Renderer.h"
#include <iostream>
#include <fstream>//读取文件
#include <sstream>//字符串流

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
	: m_vertexPath(vertexPath)
	, m_fragmentPath(fragmentPath)
{
	// 1. 从文件路径中获取顶点/片段着色器
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// 保证ifstream对象可以抛出异常：
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// 打开文件
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// 读取文件的缓冲内容到数据流中
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// 关闭文件处理器
		vShaderFile.close();
		fShaderFile.close();
		// 转换数据流到string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	m_rendererID = createShader(vertexCode, fragmentCode);
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_rendererID));
}

void Shader::bind() const
{
	GLCall(glUseProgram(m_rendererID));
}

void Shader::unbind() const
{
	GLCall(glUseProgram(0));
}

void Shader::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	GLCall(glUniform4f(getUniformLocation(name), v0, v1, v2, v3));
}

void Shader::setUniform3f(const std::string& name, float v0, float v1, float v2)
{
	GLCall(glUniform3f(getUniformLocation(name), v0, v1, v2));
}

void Shader::setUniform3f(const std::string& name, glm::vec3 v)
{
	GLCall(glUniform3f(getUniformLocation(name), v.x, v.y, v.z));
}

void Shader::setUniform1i(const std::string& name, int value)
{
	GLCall(glUniform1i(getUniformLocation(name), value));
}

void Shader::setUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
	GLCall(glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

int Shader::getUniformLocation(const std::string& name)
{
	if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end())
		return m_uniformLocationCache[name];

	//每次都调用glGetUniformLocation非常影响性能
	GLCall(int location = glGetUniformLocation(m_rendererID, name.c_str()));
	if (-1 == location)
		std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
	m_uniformLocationCache[name] = location;
	return location;
}

unsigned int Shader::compileShader(unsigned int type, const std::string& source)
{
	GLCall(unsigned int id = glCreateShader(type));

	const char* src = source.c_str();
	GLCall(glShaderSource(id, 1, &src, nullptr));
	GLCall(glCompileShader(id));

	int result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	if (GL_FALSE == result)
	{
		//查看错误原因
		int length;
		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		//alloca：分配的内存在局部函数的上下文中，当调用的函数返回的时候，会自动释放
		char *message = (char*)alloca(length * sizeof(char));
		GLCall(glGetShaderInfoLog(id, length, &length, message));
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") \
			<< " shader!" << std::endl;
		std::cout << message << std::endl;

		GLCall(glDeleteShader(id));
		return 0;

	}

	return id;
}

unsigned int Shader::createShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	GLCall(unsigned int program = glCreateProgram());

	unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	//6.删除中间生成的vertexShader和fragmentShader
	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));

	return program;
}


#include "Shader.h"
#include <glad/glad.h>
#include <iostream>
#include <fstream>//��ȡ�ļ�
#include <sstream>//�ַ�����

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
	: m_vertexPath(vertexPath)
	, m_fragmentPath(fragmentPath)
{
	// 1. ���ļ�·���л�ȡ����/Ƭ����ɫ��
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// ��֤ifstream��������׳��쳣��
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// ���ļ�
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// ��ȡ�ļ��Ļ������ݵ���������
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// �ر��ļ�������
		vShaderFile.close();
		fShaderFile.close();
		// ת����������string
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
	glDeleteProgram(m_rendererID);
}

void Shader::bind() const
{
	glUseProgram(m_rendererID);
}

void Shader::unbind() const
{
	glUseProgram(0);
}

void Shader::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	glUniform4f(getUniformLocation(name), v0, v1, v2, v3);
}

void Shader::setUniform3f(const std::string& name, float v0, float v1, float v2)
{
	glUniform3f(getUniformLocation(name), v0, v1, v2);
}

void Shader::setUniform3f(const std::string& name, glm::vec3 v)
{
	glUniform3f(getUniformLocation(name), v.x, v.y, v.z);
}

void Shader::setUniform1i(const std::string& name, int value)
{
	glUniform1i(getUniformLocation(name), value);
}

void Shader::setUniform1f(const std::string& name, float value)
{
	glUniform1f(getUniformLocation(name), value);
}

void Shader::setUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

int Shader::getUniformLocation(const std::string& name)
{
	if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end())
		return m_uniformLocationCache[name];

	//ÿ�ζ�����glGetUniformLocation�ǳ�Ӱ������
	int location = glGetUniformLocation(m_rendererID, name.c_str());
	if (-1 == location)
		std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
	m_uniformLocationCache[name] = location;
	return location;
}

unsigned int Shader::compileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);

	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (GL_FALSE == result)
	{
		//�鿴����ԭ��
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		//alloca��������ڴ��ھֲ��������������У������õĺ������ص�ʱ�򣬻��Զ��ͷ�
		char *message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") \
			<< " shader!" << std::endl;
		std::cout << message << std::endl;

		glDeleteShader(id);
		return 0;

	}

	return id;
}

unsigned int Shader::createShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();

	unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	//6.ɾ���м����ɵ�vertexShader��fragmentShader
	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

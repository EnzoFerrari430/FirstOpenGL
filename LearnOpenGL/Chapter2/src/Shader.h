#pragma once
#include <string>
#include <unordered_map>

#include "glm/glm.hpp"

class Shader
{
private:
	unsigned int m_rendererID;
	std::string m_vertexPath;
	std::string m_fragmentPath;
	std::unordered_map<std::string, int> m_uniformLocationCache;
public:
	Shader(const std::string& vertexPath, const std::string& fragmentPath);
	~Shader();

	void bind() const;
	void unbind() const;

	//set uniforms 后续可能有很多很多类似的函数SetUniform1f SetUniform2f等等 这里不赘述
	void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void setUniform3f(const std::string& name, float v0, float v1, float v2);

	void setUniform1i(const std::string& name, int nalue);

	void setUniformMat4f(const std::string& name, const glm::mat4& matrix);
private:
	unsigned int compileShader(unsigned int type, const std::string& source);
	unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader);

	int getUniformLocation(const std::string& name);
};
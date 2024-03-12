#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <unordered_map>

class Shader
{
public:
    unsigned int ID;

    // constructor genernates the shader on the fly
    Shader(const char* vertexPath, const char* fragmentPath)
    {
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;

        //确保文件流对象能抛出异常
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            vShaderFile.open(vertexPath, std::ios::in);
            fShaderFile.open(fragmentPath, std::ios::in);
            std::stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            vShaderFile.close();
            fShaderFile.close();
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        unsigned int vertex, fragment;
        // Vertex Shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, nullptr);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // Fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, nullptr);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");

        unsigned int program;
        program = glCreateProgram();
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);
        checkCompileErrors(program, "PROGRAM");
        ID = program;

        // Don't forget Delete Shader
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    Shader::~Shader()
    {
        std::cout << "Delete Shader Program: " << ID << std::endl;
        glDeleteProgram(ID);
    }

    void use() const
    {
        glUseProgram(ID);
    }

    void unuse() const
    {
        glUseProgram(0);
    }

    void setBool(const std::string& name, bool value) const
    {
        int location = getUniformLocation(name);
        glUniform1i(location, value);
    }

    void setInt(const std::string& name, int value) const
    {
        int location = getUniformLocation(name);
        glUniform1i(location, value);
    }

    void setFloat(const std::string& name, float value) const
    {
        int location = getUniformLocation(name);
        glUniform1f(location, value);
    }

    void setVec2(const std::string& name, const glm::vec2& value) const
    {
        int location = getUniformLocation(name);
        glUniform2fv(location, 1, &value[0]);
    }

    void setVec2(const std::string& name, float x, float y) const
    {
        int location = getUniformLocation(name);
        glUniform2f(location, x, y);
    }

    void setVec3(const std::string& name, const glm::vec3& value) const
    {
        int location = getUniformLocation(name);
        glUniform3fv(location, 1, &value[0]);
    }

    void setVec3(const std::string& name, float x, float y, float z) const
    {
        int location = getUniformLocation(name);
        glUniform3f(location, x, y, z);
    }

    void setVec4(const std::string& name, const glm::vec4 value) const
    {
        int location = getUniformLocation(name);
        glUniform4fv(location, 1, &value[0]);
    }

    void setVec4(const std::string& name, float x, float y, float z, float w) const
    {
        int location = getUniformLocation(name);
        glUniform4f(location, x, y, z, w);
    }

    void setMat2(const std::string& name, const glm::mat2& mat) const
    {
        int location = getUniformLocation(name);
        glUniformMatrix2fv(location, 1, GL_FALSE, &mat[0][0]); //glm也是以列为主序 故不用转置
    }

    void setMat3(const std::string& name, const glm::mat3& mat) const
    {
        int location = getUniformLocation(name);
        glUniformMatrix3fv(location, 1, GL_FALSE, &mat[0][0]);
    }

    void setMat4(const std::string& name, const glm::mat4& mat) const
    {
        int location = getUniformLocation(name);
        glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
    }

private:
    // utility function for checking shader compilation/link errors.
    void checkCompileErrors(GLuint shader, const std::string& type)
    {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " <<
                    type << "\n" << infoLog <<
                    "\n -- -------------------------------------------------------------------- --" << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
                std::cout<<"ERROR::PROGRAM_LINKING_ERROR of type: " <<
                    type << "\n" << infoLog <<
                    "\n -- -------------------------------------------------------------------- --" << std::endl;
            }
        }
    }

    int getUniformLocation(const std::string& name) const
    {
        if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end())
        {
            return m_uniformLocationCache[name];
        }

        int location = glGetUniformLocation(ID, name.c_str());
        if (-1 == location)
        {
            std::cout << "ERROR::GET_UNIFORM_ERROR (" << name << ") doesn't exist!" << std::endl;
        }
        m_uniformLocationCache[name] = location;
        return location;
    }

private:
    mutable std::unordered_map<std::string, int> m_uniformLocationCache;
};

#endif
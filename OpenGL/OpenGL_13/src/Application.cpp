/*
p13:
	��ʼ��װ��
	��װ vertex buffer object ��vertex index buffer
*/

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

#include <fstream>//��ȡ�ļ�
#include <sstream>//�ַ�����

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"


#pragma region glfw�� ������
void glfwErrorHandle(int errCode, const char* errMsg)
{
	std::cout << "[glfw error] (" << errCode << ") :" << errMsg << std::endl;
}
#pragma endregion


#pragma region ������ɫ������
struct shaderProgramSource
{
	std::string vertexSource;
	std::string fragmentSource;
};

//��Ϊ����ֻ��һ����OpenGL�̳̣�����ʹ��C++����ȡ�ļ�
//C++����ȡ�ļ����C����API��һЩ
static shaderProgramSource parseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class shaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];//2���ַ����� һ����������vertexShader һ����������fragmentShader
	shaderType type = shaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = shaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = shaderType::FRAGMENT;
		}
		else
		{
			// TODO: type == shaderType::NONE������
			ss[(int)type] << line << "\n";
		}
	}
	return { ss[0].str(), ss[1].str() };
}

static unsigned int compileShader(unsigned int type, const std::string& source)
{
	GLCall(unsigned int id = glCreateShader(type));

	const char* src = source.c_str();
	GLCall(glShaderSource(id, 1, &src, nullptr));
	GLCall(glCompileShader(id));

	int result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	if (GL_FALSE == result)
	{
		//�鿴����ԭ��
		int length;
		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		//alloca��������ڴ��ھֲ��������������У������õĺ������ص�ʱ�򣬻��Զ��ͷ�
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

static int createShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	GLCall(unsigned int program = glCreateProgram());

	unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	//6.ɾ���м����ɵ�vertexShader��fragmentShader
	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));

	return program;
}
#pragma endregion

int main()
{
	GLFWwindow* window;

	glfwSetErrorCallback(glfwErrorHandle);

	if (!glfwInit())
	{
		return -1;
	}

	//ʹ��OpenGL����ģʽ
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//Ĭ�����е��Ǽ���ģʽ

	window = glfwCreateWindow(640, 480, "EnzoWindow", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);

	//����Ҫ��glew���ʼ��֮��ſ��Ե���glGetError
	if (glewInit() != GLEW_OK)
	{
		std::cout << "error" << std::endl;
	}

	GLCall(std::cout << glGetString(GL_VERSION) << std::endl);

	//������������VertexBuffer IndexBuffer����glfwTerminate֮ǰ����
	{
		//��������
		float positions[] = {
			-0.5f, -0.5f, // 0
			 0.5f, -0.5f, // 1
			 0.5f,  0.5f, // 2
			-0.5f,  0.5f  // 3
		};

		//ָʾ�����4������������ν��л���
		unsigned int indices[] = {
			0, 1, 2, //��һ�������ε�3����������
			2, 3, 0  //�ڶ��������ε�3����������
		};

		VertexArray va;
		VertexBuffer vb(positions, 4 * 2 * sizeof(float));
		VertexBufferLayout layout;
		/*
		����ж�����ԣ�ȷ����AddBuffer֮ǰ��push��ȥ
		��AddBuffer��ʱ������Ҫ����stride
		*/
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);

		IndexBuffer ib(indices, 6);

		//X:5����shader
		//X:5.1  ������ɫ��Դ��
		shaderProgramSource source = parseShader("res/shaders/Basic.shader");

		unsigned int shader = createShader(source.vertexSource, source.fragmentSource);

		GLCall(glUseProgram(shader));

		///////////////////////////////////////////////////
		//p11:�ڳ���ʹ����ɫ������֮��  ���Ե���uniform������ɫ

		//1.�ҵ�uniform ID,ʧ�ܷ���-1
		GLCall(int location = glGetUniformLocation(shader, "u_Color"));
		ASSERT(location != -1);
		///////////////////////////////////////////////////

		//���,����Ⱦ��ʱ���ٰ���
		GLCall(glUseProgram(0));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
		GLCall(glBindVertexArray(0));

		//��̬�仯��ɫ
		float r = 0.0f;
		float increment = 0.05f;//����

		while (!glfwWindowShouldClose(window))
		{
			glClear(GL_COLOR_BUFFER_BIT);

			//ʹ�ö������ݶ���
			GLCall(glUseProgram(shader));
			GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));
			
			va.Bind();
			ib.Bind();

			///drawϵ�еĺ���������Ⱦ�Ĳ��� �������������Ⱦ��������
			GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

			GLCall(glBindVertexArray(0));


			if (r > 1.0f)
				increment = -0.05f;
			else if (r < 0.0f)
				increment = 0.05f;
			r += increment;

			glfwSwapBuffers(window);


			glfwPollEvents();
		}

		glDeleteProgram(shader);
	}
	glfwTerminate();
	//ɾ��glfw������֮��glGetError�᷵�ش���
	//ÿ�ε������glGetError�����ش������������ѭ��
	//����ǳ����˲�������ԭ��(���������ﲡ!!!  �����Ϊʲô...��ԭ��)
	return 0;
}
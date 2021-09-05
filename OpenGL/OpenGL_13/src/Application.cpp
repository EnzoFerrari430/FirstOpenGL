/*
p13:
	开始封装类
	封装 vertex buffer object 和vertex index buffer
*/

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

#include <fstream>//读取文件
#include <sstream>//字符串流

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"


#pragma region glfw库 错误处理
void glfwErrorHandle(int errCode, const char* errMsg)
{
	std::cout << "[glfw error] (" << errCode << ") :" << errMsg << std::endl;
}
#pragma endregion


#pragma region 解析着色器程序
struct shaderProgramSource
{
	std::string vertexSource;
	std::string fragmentSource;
};

//因为这里只是一个简单OpenGL教程，所以使用C++风格读取文件
//C++风格读取文件会比C风格的API慢一些
static shaderProgramSource parseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class shaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];//2个字符串流 一个用来保存vertexShader 一个用来保存fragmentShader
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
			// TODO: type == shaderType::NONE就凉了
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

static int createShader(const std::string& vertexShader, const std::string& fragmentShader)
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
#pragma endregion

int main()
{
	GLFWwindow* window;

	glfwSetErrorCallback(glfwErrorHandle);

	if (!glfwInit())
	{
		return -1;
	}

	//使用OpenGL核心模式
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//默认运行的是兼容模式

	window = glfwCreateWindow(640, 480, "EnzoWindow", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);

	//必须要在glew库初始化之后才可以调用glGetError
	if (glewInit() != GLEW_OK)
	{
		std::cout << "error" << std::endl;
	}

	GLCall(std::cout << glGetString(GL_VERSION) << std::endl);

	//增加作用域，让VertexBuffer IndexBuffer能再glfwTerminate之前析构
	{
		//顶点数据
		float positions[] = {
			-0.5f, -0.5f, // 0
			 0.5f, -0.5f, // 1
			 0.5f,  0.5f, // 2
			-0.5f,  0.5f  // 3
		};

		//指示上面的4个顶点数据如何进行绘制
		unsigned int indices[] = {
			0, 1, 2, //第一个三角形的3个顶点索引
			2, 3, 0  //第二个三角形的3个顶点索引
		};

		VertexArray va;
		VertexBuffer vb(positions, 4 * 2 * sizeof(float));
		VertexBufferLayout layout;
		/*
		如果有多个属性，确保在AddBuffer之前都push进去
		在AddBuffer的时候我们要计算stride
		*/
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);

		IndexBuffer ib(indices, 6);

		//X:5生成shader
		//X:5.1  顶点着色器源码
		shaderProgramSource source = parseShader("res/shaders/Basic.shader");

		unsigned int shader = createShader(source.vertexSource, source.fragmentSource);

		GLCall(glUseProgram(shader));

		///////////////////////////////////////////////////
		//p11:在程序使用着色器对象之后  可以调用uniform设置颜色

		//1.找到uniform ID,失败返回-1
		GLCall(int location = glGetUniformLocation(shader, "u_Color"));
		ASSERT(location != -1);
		///////////////////////////////////////////////////

		//解绑,在渲染的时候再绑定上
		GLCall(glUseProgram(0));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
		GLCall(glBindVertexArray(0));

		//动态变化颜色
		float r = 0.0f;
		float increment = 0.05f;//步长

		while (!glfwWindowShouldClose(window))
		{
			glClear(GL_COLOR_BUFFER_BIT);

			//使用顶点数据对象
			GLCall(glUseProgram(shader));
			GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));
			
			va.Bind();
			ib.Bind();

			///draw系列的函数都是渲染的操作 这个函数交给渲染类来处理
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
	//删除glfw上下文之后，glGetError会返回错误
	//每次调用这个glGetError都返回错误，造成了无限循环
	//这就是程序退不出来的原因(经典中文语病!!!  这就是为什么...的原因)
	return 0;
}
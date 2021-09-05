/*
p13:
	开始封装类
	封装 vertex buffer object 和vertex index buffer
*/

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"


#pragma region glfw库 错误处理
void glfwErrorHandle(int errCode, const char* errMsg)
{
	std::cout << "[glfw error] (" << errCode << ") :" << errMsg << std::endl;
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

		Shader shader("res/shaders/Basic.shader");
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

		//解绑,在渲染的时候再绑定上
		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();

		//动态变化颜色
		float r = 0.0f;
		float increment = 0.05f;//步长

		while (!glfwWindowShouldClose(window))
		{
			glClear(GL_COLOR_BUFFER_BIT);

			//使用顶点数据对象
			shader.Bind();
			shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
			
			va.Bind();
			ib.Bind();

			///draw系列的函数都是渲染的操作 这个函数交给渲染类来处理
			GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

			va.Unbind();
			ib.Unbind();
			shader.Unbind();

			if (r > 1.0f)
				increment = -0.05f;
			else if (r < 0.0f)
				increment = 0.05f;
			r += increment;

			glfwSwapBuffers(window);

			glfwPollEvents();
		}

		//离开作用域  析构 shader va vb ib
	}
	glfwTerminate();
	//删除glfw上下文之后，glGetError会返回错误
	//每次调用这个glGetError都返回错误，造成了无限循环
	//这就是程序退不出来的原因(经典中文语病!!!  这就是为什么...的原因)
	return 0;
}
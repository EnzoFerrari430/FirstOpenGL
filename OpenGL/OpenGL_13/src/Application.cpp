/*
p13:
	开始封装类
	封装 vertex buffer object 和vertex index buffer

p18:
	blend 混合
	混合就是将我们的源色和我们的目标色以某种混合生成特效的技术
	混合操作只能在RGBA模式下进行
	混合的关键作用的a值上将源色和目标色按照给定的比率进行混合，以达到不同程度的透明度。
	a值为0则完全透明 a值为1则完全不透明。

	源色 source color：fragment shader中输出的最终颜色
	目标色 target color: fragment shader输出的区域的颜色

	默认混合值是 源色1即 GL_ONE 目标色0 GL_ZERO

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);// 是最常使用的
	
	例子:
	R = (src.r * 0) + (dst.r * (1 - 0)) = dst.r;
	G = (src.g * 0) + (dst.g * (1 - 0)) = dst.g;
	B = (src.b * 0) + (dst.b * (1 - 0)) = dst.b;
	A = (src.a * 0) + (dst.a * (1 - 0)) = dst.a;
*/

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexBufferLayout.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#pragma region glfw库 错误处理
void glfwErrorHandle(int errCode, const char* errMsg)
{
	std::cout << "[glfw error] (" << errCode << ") :" << errMsg << std::endl;
}
#pragma endregion

#pragma region 打印数据
//打印矩阵数据
void printMatrix4(const glm::mat4 matrix)
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			std::cout << matrix[j][i] << "\t\t";
		}
		std::cout << std::endl;
	}
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

	window = glfwCreateWindow(900, 540, "EnzoWindow", NULL, NULL);
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
			//位置		  //纹理坐标
			//-0.5f, -0.5f, 0.0f, 0.0f,// 0
			// 0.5f, -0.5f, 1.0f, 0.0f,// 1
			// 0.5f,  0.5f, 1.0f, 1.0f,// 2
			//-0.5f,  0.5f, 0.0f, 1.0f, // 3
			100.0f, 100.0f, 0.0f, 0.0f,// 0
			200.0f, 100.0f, 1.0f, 0.0f,// 1
			200.0f, 200.0f, 1.0f, 1.0f,// 2
			100.0f, 200.0f, 0.0f, 1.0f // 3
		};

		//指示上面的4个顶点数据如何进行绘制
		unsigned int indices[] = {
			0, 1, 2, //第一个三角形的3个顶点索引
			2, 3, 0  //第二个三角形的3个顶点索引
		};

		//blend混合
		//这里禁用混合，可以看清楚一点矩阵的变换
		//GLCall(glEnable(GL_BLEND));
		//GLCall(glBlendFunc(GL_ONE, GL_ZERO));
		//GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		VertexArray va;
		VertexBuffer vb(positions, 4 * 4 * sizeof(float));
		VertexBufferLayout layout;
		/*
		如果有多个属性，确保在AddBuffer之前都push进去
		在AddBuffer的时候我们要计算stride
		*/
		layout.Push<float>(2);
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);

		IndexBuffer ib(indices, 6);

		//一个优秀的缩放矩阵
		//既可以填入标准化坐标，也可以填入屏幕坐标
		//可以根据传入的左右 上下值构造一个缩放比例
		glm::mat4 proj = glm::ortho(0.0f, 900.0f, 0.0f, 540.0f, -1.0f, 1.0f);
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-100.0f, 0.0f, 0.0f));//平移矩阵
		//glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(200.0f, 200.0f, 0.0f));//平移矩阵

		//printMatrix4(proj);
		//printMatrix4(view);
		//printMatrix4(model);

		//glm::mat4 mvp = proj * view * model; //可以选择交给CPU计算也可以通过uniform交给GPU计算
		//printMatrix4(mvp);

		Shader shader("res/shaders/Basic.shader");
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);
		//shader.SetUniformMat4f("u_MVP", mvp);
		//shader.SetUniformMat4f("u_Model", model);
		//shader.SetUniformMat4f("u_View", view);
		//shader.SetUniformMat4f("u_Proj", proj);

		//要保证这里的SetUniform1i设置的0与上面绑定的激活纹理单元一致
		//这样u_Texture表示的纹理采样器才能和纹理对象中保存的图片信息保持一致
		Texture texture("res/textures/MyLogo.png");
		texture.Bind();//绑定要纹理单元0上
		shader.SetUniform1i("u_Texture", 0);

		//解绑,在渲染的时候再绑定上
		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();

		//创建一个Renderer类
		Renderer renderer;


		//imgui
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init();

		glm::vec3 translation(200.0f, 200.0f, 0.0f);

		//动态变化颜色
		float r = 0.0f;
		float increment = 0.05f;//步长

		while (!glfwWindowShouldClose(window))
		{
			renderer.setBGColor(0.2f, 0.3f, 0.3f, 1.0f);
			renderer.Clear();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
			glm::mat4 mvp = proj * view * model;

			//使用顶点数据对象
			shader.Bind();
			shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
			shader.SetUniformMat4f("u_MVP", mvp);

			renderer.Draw(va, ib, shader);

			//p16:unBind不是必要的操作，能再调试时起作用，但是会影响性能
			//va.Unbind();
			//ib.Unbind();
			//shader.Unbind();

			if (r > 1.0f)
				increment = -0.05f;
			else if (r < 0.0f)
				increment = 0.05f;
			r += increment;

			{
				ImGui::SliderFloat3("Translation", &translation.x, 0.0f, 960.0f);
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(window);

			glfwPollEvents();
		}

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		//离开作用域  析构 shader va vb ib
	}



	glfwTerminate();
	//删除glfw上下文之后，glGetError会返回错误
	//每次调用这个glGetError都返回错误，造成了无限循环
	//这就是程序退不出来的原因(经典中文语病!!!  这就是为什么...的原因)
	return 0;
}
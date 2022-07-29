#include "camera.h"
#include <glad/glad.h>	//该头文件因为包含了正确的OpenGL头文件  所以需要在glfw3.h之前包含
#include <GLFW/glfw3.h>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "Shader.h"

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>

//回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height); 
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

float lastX = 800.0f / 2.0;
float lastY = 600.0f / 2.0;
bool firstMove = true;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
	glfwInit();//初始化
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Enzo Window", NULL, NULL);
	if (NULL == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	//指定窗口window为glfw的设备上下文，就可以操作窗口了
	glfwMakeContextCurrent(window);

	//注册回调
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//注册鼠标事件
	//glfw窗口的(0,0)坐标是在左上角
	glfwSetCursorPosCallback(window, mouse_callback);
	//鼠标滚轮事件
	glfwSetScrollCallback(window, scroll_callback);

	//设置光标模式
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//初始化GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}


	//顶点数据
	float vertexs[] = {
		//坐标                //法线
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,//后面
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,//前面
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,//左面
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,//右面
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,//底面
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,//顶面
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	{
		GLCall(glEnable(GL_DEPTH_TEST));

		//局部域
		//自动析构
		VertexArray va;
		va.bind();

		VertexBuffer vb(vertexs, sizeof(vertexs));
		VertexBufferLayout layout;
		layout.push<float>(3); //坐标
		layout.push<float>(3); //法线
		va.addBuffer(vb, layout);

		VertexArray lightVa;
		lightVa.bind();
		lightVa.addBuffer(vb, layout);


		Shader shader("res/shader/vertex.shader", "res/shader/fragment.shader");
		Shader lightShader("res/shader/vertex.shader", "res/shader/lightFrag.shader");


		//解绑
		va.unbind();
		vb.unbind();
		lightVa.unbind();
		shader.unbind();
		lightShader.unbind();


		Renderer renderer;

		while (!glfwWindowShouldClose(window))
		{
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
			// 输入
			processInput(window);

			renderer.setBGColor(0.1f, 0.1f, 0.1f, 1.0f);
			renderer.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			glm::mat4 view = camera.GetViewMatrix();
			{
				shader.bind();

				shader.setUniform3f("objectColor", 1.0f, 0.5f, 0.31f);
				shader.setUniform3f("lightColor", 1.0f, 1.0f, 1.0f);
				lightPos.x = 2.33f * sin(currentFrame);
				lightPos.z = 2.33f * cos(currentFrame);

				shader.setUniform3f("lightPos", lightPos);
				shader.setUniform3f("viewPos", camera.Position);

				shader.setUniformMat4f("projection", projection);
				shader.setUniformMat4f("view", view);

				// world transformation
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));
				shader.setUniformMat4f("model", model);

				//renderer.Draw(va, ib, shader);
				renderer.draw(va, 0, 36, shader);
			}

			{
				lightShader.bind();

				lightShader.setUniform3f("lightColor", 1.0f, 1.0f, 1.0f);

				lightShader.setUniformMat4f("projection", projection);
				lightShader.setUniformMat4f("view", view);

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, lightPos);
				model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
				lightShader.setUniformMat4f("model", model);

				renderer.draw(lightVa, 0, 36, lightShader);
			}

			glfwSwapBuffers(window);
			glfwPollEvents();//检查触发事件
		}

	}


	glfwTerminate();//反初始化
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

/*

	template<typename T, qualifier Q>
	GLM_FUNC_DECL mat<4, 4, T, Q> lookAt(
		vec<3, T, Q> const& eye, vec<3, T, Q> const& center, vec<3, T, Q> const& up);
	相机的方向向量从center指向eye，即 eye - center
	
	glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp)
	cameraPos - (cameraPos + cameraFront) = -cameraFront
	



这里的front.z初始要等于-1所以这里只能等于cos(yaw)
在初始yaw等于0.0f的情况下 只能用-cos(yaw)表示
*/
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMove)
	{
		lastX = xpos;
		lastY = ypos;
		firstMove = false;
	}

	float xoffset = xpos - lastX;//鼠标左移 xpos变小 视角要左转(相机逆时针 yaw变大)
	float yoffset = lastY - ypos;//鼠标上移 ypos变小 视角要上仰(相机从下面往上看 ZOY平面是个顺时针移动 pitch变小)
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

#include <glad/glad.h>	//该头文件因为包含了正确的OpenGL头文件  所以需要在glfw3.h之前包含
#include <GLFW/glfw3.h>

#include <VertexArray.h>
#include <IndexBuffer.h>
#include <VertexBufferLayout.h>
#include <Texture.h>
#include <Shader.h>

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

/*
定位一个摄像机(摄像机方向 + 摄像机位置)
代表摄像机方向的向量
右向量 上向量 方向向量(这里的方向向量是相反的 看向-Z轴，方向就是正Z轴方向)
摄像机位置向量
*/
glm::vec3 cameraPos		= glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront	= glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp		= glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

float yaw = 0.0f;//偏航角  摄像机初始在正X轴 -90度之后就在正Z轴上了
float pitch = 0.0f;//俯仰角
float lastX = 800.0f / 2.0;
float lastY = 600.0f / 2.0;
float fov = 45.0f;
bool firstMove = true;

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
		//坐标				//纹理
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,//后面
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,//前面
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,//左面
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,//右面
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,//底面
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,//顶面
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	unsigned int indicies[] = {
		0, 1, 3, //第一个三角形
		1, 2, 3  //第二个三角形
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
		layout.push<float>(2); //纹理
		va.addBuffer(vb, layout);

		IndexBuffer ib(indicies, 6);

		Texture texture1("res/pic/container.jpg");
		Texture texture2("res/pic/awesomeface.png");

		Shader shader("res/shader/vertex.shader", "res/shader/fragment.shader");

		//glm
		//旋转
		glm::mat4 model(1.0f);
		model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//平移
		glm::mat4 view(1.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		////透视投影
		//glm::mat4 proj(1.0f);
		//proj = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		shader.bind();
		texture1.bind(0);
		texture2.bind(1);
		shader.setUniform1i("texture1", 0);
		shader.setUniform1i("texture2", 1);
		shader.setUniformMat4f("model", model);
		shader.setUniformMat4f("view", view);
		//shader.setUniformMat4f("projection", proj);





		//解绑
		va.unbind();
		vb.unbind();
		ib.unbind();
		shader.unbind();

		//先旋转再位移
		glm::vec3 cubePositions[] = {
			glm::vec3( 0.0f,  0.0f,   0.0f),
			glm::vec3( 2.0f,  5.0f, -15.0f),
			glm::vec3(-1.5f, -2.2f,  -2.5f),
			glm::vec3(-3.8f, -2.0f, -12.3f),
			glm::vec3( 2.4f, -0.4f,  -3.5f),
			glm::vec3(-1.7f,  3.0f,  -7.5f),
			glm::vec3( 1.3f, -2.0f,  -2.5f),
			glm::vec3( 1.5f,  2.0f,  -2.5f),
			glm::vec3( 1.5f,  0.2f,  -1.5f),
			glm::vec3(-1.3f,  1.0f,  -1.5f)
		};


		//
		Renderer renderer;

		while (!glfwWindowShouldClose(window))
		{
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
			// 输入
			processInput(window);

			{
				renderer.setBGColor(0.2f, 0.3f, 0.3f, 1.0f);
				renderer.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				shader.bind();

				//透视投影
				glm::mat4 proj(1.0f);
				proj = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

				shader.setUniformMat4f("projection", proj);

				texture1.bind(0);
				texture2.bind(1);
				for (int i = 0; i < 10; ++i)
				{
					model = glm::mat4(1.0f);
					model = glm::translate(model, cubePositions[i]);
					float angle = 20.0 * i;
					if (i % 3 == 0)
					{
						angle = glfwGetTime() * (20 + 10 * i);
					}
					model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
					shader.setUniformMat4f("model", model);

					view = glm::mat4(1.0f);
					view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
					shader.setUniformMat4f("view", view);
					renderer.draw(va, 0, 36, shader);
				}
				//model = glm::mat4(1.0f);//初始化为单位矩阵
				//model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
				//shader.SetUniformMat4f("model", model);

				//renderer.Draw(va, ib, shader);
				//renderer.Draw(va, 0, 36, shader);


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
	{
		glfwSetWindowShouldClose(window, true);
	}

	float cameraSpeed = 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		//只减Z方向的分量
		cameraPos += cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		cameraPos -= cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
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

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front(1.0f);
	front.x = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = -cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}

#include "camera.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "Shader.h"

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <string>
#include <fmt/core.h>

struct PointLight {
	glm::vec3 position;

	float constant;//衰减常量
	float linear;//衰减1次项
	float quadratic;//衰减2次项

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

//回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height); 
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

/*
Brief: set point light uniforms
*/
void setPointLightUniform(int index, const PointLight& pointLight, Shader& shader);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

float lastX = 800.0f / 2.0;
float lastY = 600.0f / 2.0;
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
		//坐标                //法线                //贴图
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,//后面
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,//前面
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,//左面
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,//右面
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,//底面
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,//顶面
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
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
		layout.push<float>(2); //贴图
		va.addBuffer(vb, layout);

		VertexArray lightVa;
		lightVa.bind();
		lightVa.addBuffer(vb, layout);

		Texture texture1("res/pic/container2.png");
		Texture texture2("res/pic/container2_specular.png");


		Shader shader("res/shader/vertex.shader", "res/shader/fragment.shader");
		Shader lightShader("res/shader/vertex.shader", "res/shader/lightFrag.shader");


		//解绑
		va.unbind();
		vb.unbind();
		lightVa.unbind();
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


		//定向光
		glm::vec3 dirPos(-0.2f, -1.0f, -0.3f);
		glm::vec3 dirLightColor(0.5f);
		glm::vec3 dirLightDiffuseColor = dirLightColor * glm::vec3(0.8f);
		glm::vec3 dirLightAmbientColor = dirLightDiffuseColor * glm::vec3(0.1f);

		//点光源
		glm::vec3 lightColors[] = {
			glm::vec3(1.0f, 0.6f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(0.2f, 0.2f, 1.0f)
		};
		glm::vec3 pointLightPositions[] = {
			glm::vec3( 0.7f,  0.2f,  2.0f),
			glm::vec3( 2.3f, -3.3f, -4.0f),
			glm::vec3(-4.0f,  2.0f, -12.0f),
			glm::vec3( 0.0f,  0.0f, -3.0f)
		};
		PointLight samplePointLight;
		//samplePointLight.specular = dirLightColor;
		//samplePointLight.diffuse = dirLightDiffuseColor;
		//samplePointLight.ambient = dirLightAmbientColor;
		samplePointLight.constant = 1.0f;
		samplePointLight.linear = 0.09f;
		samplePointLight.quadratic = 0.032f;



		Renderer renderer;

		while (!glfwWindowShouldClose(window))
		{
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
			// 输入
			processInput(window);

			renderer.setBGColor(0.75f, 0.52f, 0.3f, 1.0f);
			renderer.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			glm::mat4 view = camera.GetViewMatrix();
			{
				shader.bind();
				texture1.bind(0);
				texture2.bind(1);

				shader.setUniformMat4f("projection", projection);
				shader.setUniformMat4f("view", view);

				shader.setUniform3f("viewPos", camera.Position);

				shader.setUniform1i("material.diffuse", 0);
				shader.setUniform1i("material.specular", 1);
				shader.setUniform1f("material.shininess", 128.0f);

				shader.setUniform3f("dirLight.ambient", dirLightAmbientColor);
				shader.setUniform3f("dirLight.diffuse", dirLightDiffuseColor);
				shader.setUniform3f("dirLight.specular", dirLightColor);
				shader.setUniform3f("dirLight.direction", dirPos);

				//聚光
				glm::vec3 lightColor(1.0f);
				//lightColor.r = sin(glfwGetTime() * 2.0f);
				//lightColor.g = sin(glfwGetTime() * 0.7f);
				//lightColor.b = sin(glfwGetTime() * 1.3f);
				glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
				glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
				shader.setUniform3f("spotLight.position", camera.Position);
				shader.setUniform3f("spotLight.direction", camera.Front);
				shader.setUniform1f("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
				shader.setUniform1f("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
				shader.setUniform3f("spotLight.ambient", ambientColor);
				shader.setUniform3f("spotLight.diffuse", diffuseColor);
				shader.setUniform3f("spotLight.specular", lightColor);
				shader.setUniform1f("spotLight.constant", 1.0f);
				shader.setUniform1f("spotLight.linear", 0.09f);
				shader.setUniform1f("spotLight.quadratic", 0.032f);


				//点光源
				for (int i = 0; i < 4; ++i) {
					samplePointLight.position = pointLightPositions[i];
					samplePointLight.specular = lightColors[i];
					samplePointLight.diffuse = samplePointLight.specular * glm::vec3(1.0f);
					samplePointLight.ambient = samplePointLight.specular * glm::vec3(0.1f);
					setPointLightUniform(i, samplePointLight, shader);
				}

				// world transformation
				for (int i = 0; i < 10; ++i) {
					glm::mat4 model = glm::mat4(1.0f);
					model = glm::translate(model, cubePositions[i]);
					float angle = 20.0 * i;
					if (i % 3 == 0)
					{
						angle = glfwGetTime() * (20 + 10 * i);
					}
					model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
					shader.setUniformMat4f("model", model);

					renderer.draw(va, 0, 36, shader);
				}

				lightShader.bind();
				lightShader.setUniformMat4f("projection", projection);
				lightShader.setUniformMat4f("view", view);
				for (int i = 0; i < 4; ++i) {
					glm::mat4 model = glm::mat4(1.0f);
					model = glm::translate(model, pointLightPositions[i]);
					model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
					lightShader.setUniform3f("lightColor", lightColors[i]);
					lightShader.setUniformMat4f("model", model);
					renderer.draw(lightVa, 0, 36, lightShader);
				}
				
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


void setPointLightUniform(int index, const PointLight& pointLight, Shader& shader)
{
	shader.setUniform3f(fmt::format("pointLights[{}].position", index).c_str(), pointLight.position);
	shader.setUniform3f(fmt::format("pointLights[{}].ambient", index).c_str(), pointLight.ambient);
	shader.setUniform3f(fmt::format("pointLights[{}].diffuse", index).c_str(), pointLight.diffuse);
	shader.setUniform3f(fmt::format("pointLights[{}].specular", index).c_str(), pointLight.specular);
	shader.setUniform1f(fmt::format("pointLights[{}].constant", index).c_str(), pointLight.constant);
	shader.setUniform1f(fmt::format("pointLights[{}].linear", index).c_str(), pointLight.linear);
	shader.setUniform1f(fmt::format("pointLights[{}].quadratic", index).c_str(), pointLight.quadratic);
}

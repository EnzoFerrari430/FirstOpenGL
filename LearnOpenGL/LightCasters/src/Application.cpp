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

//�ص�����
void framebuffer_size_callback(GLFWwindow* window, int width, int height); 
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f; // ��һ֡��ʱ��

float lastX = 800.0f / 2.0;
float lastY = 600.0f / 2.0;
bool firstMove = true;

// lighting
glm::vec3 lightDir(-0.2f, -1.0f, -0.3f);

int main()
{
	glfwInit();//��ʼ��
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

	//ָ������windowΪglfw���豸�����ģ��Ϳ��Բ���������
	glfwMakeContextCurrent(window);

	//ע��ص�
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//ע������¼�
	//glfw���ڵ�(0,0)�����������Ͻ�
	glfwSetCursorPosCallback(window, mouse_callback);
	//�������¼�
	glfwSetScrollCallback(window, scroll_callback);

	//���ù��ģʽ
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//��ʼ��GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}


	//��������
	float vertexs[] = {
		//����                //����                //��ͼ
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,//����
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,//ǰ��
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,//����
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,//����
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,//����
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,//����
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	{
		GLCall(glEnable(GL_DEPTH_TEST));

		//�ֲ���
		//�Զ�����
		VertexArray va;
		va.bind();

		VertexBuffer vb(vertexs, sizeof(vertexs));
		VertexBufferLayout layout;
		layout.push<float>(3); //����
		layout.push<float>(3); //����
		layout.push<float>(2); //��ͼ
		va.addBuffer(vb, layout);

		VertexArray lightVa;
		lightVa.bind();
		lightVa.addBuffer(vb, layout);

		Texture texture1("res/pic/container2.png");
		Texture texture2("res/pic/container2_specular.png");


		Shader shader("res/shader/vertex.shader", "res/shader/fragment.shader");


		//���
		va.unbind();
		vb.unbind();
		lightVa.unbind();
		shader.unbind();

		//����ת��λ��
		glm::vec3 cubePositions[] = {
			glm::vec3(0.0f,  0.0f,   0.0f),
			glm::vec3(2.0f,  5.0f, -15.0f),
			glm::vec3(-1.5f, -2.2f,  -2.5f),
			glm::vec3(-3.8f, -2.0f, -12.3f),
			glm::vec3(2.4f, -0.4f,  -3.5f),
			glm::vec3(-1.7f,  3.0f,  -7.5f),
			glm::vec3(1.3f, -2.0f,  -2.5f),
			glm::vec3(1.5f,  2.0f,  -2.5f),
			glm::vec3(1.5f,  0.2f,  -1.5f),
			glm::vec3(-1.3f,  1.0f,  -1.5f)
		};


		Renderer renderer;

		while (!glfwWindowShouldClose(window))
		{
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
			// ����
			processInput(window);

			renderer.setBGColor(0.1f, 0.1f, 0.1f, 1.0f);
			renderer.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			glm::mat4 view = camera.GetViewMatrix();
			{
				shader.bind();
				texture1.bind(0);
				texture2.bind(1);

				shader.setUniform3f("viewPos", camera.Position);

				shader.setUniform1i("material.diffuse", 0);
				shader.setUniform1i("material.specular", 1);
				shader.setUniform1f("material.shininess", 128.0f);

				//��������
				glm::vec3 lightColor(1.0f);
				//lightColor.r = sin(glfwGetTime() * 2.0f);
				//lightColor.g = sin(glfwGetTime() * 0.7f);
				//lightColor.b = sin(glfwGetTime() * 1.3f);
				glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
				glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
				shader.setUniform3f("light.direction", lightDir);
				shader.setUniform3f("light.ambient", ambientColor);
				shader.setUniform3f("light.diffuse", diffuseColor);
				shader.setUniform3f("light.specular", lightColor);

				shader.setUniformMat4f("projection", projection);
				shader.setUniformMat4f("view", view);

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
			}

			glfwSwapBuffers(window);
			glfwPollEvents();//��鴥���¼�
		}

	}


	glfwTerminate();//����ʼ��
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
	����ķ���������centerָ��eye���� eye - center
	
	glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp)
	cameraPos - (cameraPos + cameraFront) = -cameraFront
	



�����front.z��ʼҪ����-1��������ֻ�ܵ���cos(yaw)
�ڳ�ʼyaw����0.0f������� ֻ����-cos(yaw)��ʾ
*/
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMove)
	{
		lastX = xpos;
		lastY = ypos;
		firstMove = false;
	}

	float xoffset = xpos - lastX;//������� xpos��С �ӽ�Ҫ��ת(�����ʱ�� yaw���)
	float yoffset = lastY - ypos;//������� ypos��С �ӽ�Ҫ����(������������Ͽ� ZOYƽ���Ǹ�˳ʱ���ƶ� pitch��С)
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

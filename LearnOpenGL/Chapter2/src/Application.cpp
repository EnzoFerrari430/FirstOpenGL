#include "camera.h"
#include <glad/glad.h>	//��ͷ�ļ���Ϊ��������ȷ��OpenGLͷ�ļ�  ������Ҫ��glfw3.h֮ǰ����
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
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

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
		//����                //����
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,//����
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,//ǰ��
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,//����
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,//����
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,//����
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,//����
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
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
		va.addBuffer(vb, layout);

		VertexArray lightVa;
		lightVa.bind();
		lightVa.addBuffer(vb, layout);


		Shader shader("res/shader/vertex.shader", "res/shader/fragment.shader");
		Shader lightShader("res/shader/vertex.shader", "res/shader/lightFrag.shader");


		//���
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
			// ����
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

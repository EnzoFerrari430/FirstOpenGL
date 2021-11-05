#include <glad/glad.h>	//��ͷ�ļ���Ϊ��������ȷ��OpenGLͷ�ļ�  ������Ҫ��glfw3.h֮ǰ����
#include <GLFW/glfw3.h>

#include <VertexArray.h>
#include <IndexBuffer.h>
#include <VertexBufferLayout.h>
#include <Texture.h>
#include <Shader.h>

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

/*
��λһ�������(��������� + �����λ��)
������������������
������ ������ ��������(����ķ����������෴�� ����-Z�ᣬ���������Z�᷽��)
�����λ������
*/
glm::vec3 cameraPos		= glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront	= glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp		= glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f; // ��һ֡��ʱ��

float yaw = 0.0f;//ƫ����  �������ʼ����X�� -90��֮�������Z������
float pitch = 0.0f;//������
float lastX = 800.0f / 2.0;
float lastY = 600.0f / 2.0;
float fov = 45.0f;
bool firstMove = true;

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
		//����				//����
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,//����
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,//ǰ��
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,//����
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,//����
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,//����
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,//����
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	unsigned int indicies[] = {
		0, 1, 3, //��һ��������
		1, 2, 3  //�ڶ���������
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
		layout.push<float>(2); //����
		va.addBuffer(vb, layout);

		IndexBuffer ib(indicies, 6);

		Texture texture1("res/pic/container.jpg");
		Texture texture2("res/pic/awesomeface.png");

		Shader shader("res/shader/vertex.shader", "res/shader/fragment.shader");

		//glm
		//��ת
		glm::mat4 model(1.0f);
		model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//ƽ��
		glm::mat4 view(1.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		////͸��ͶӰ
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





		//���
		va.unbind();
		vb.unbind();
		ib.unbind();
		shader.unbind();

		//����ת��λ��
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
			// ����
			processInput(window);

			{
				renderer.setBGColor(0.2f, 0.3f, 0.3f, 1.0f);
				renderer.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				shader.bind();

				//͸��ͶӰ
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
				//model = glm::mat4(1.0f);//��ʼ��Ϊ��λ����
				//model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
				//shader.SetUniformMat4f("model", model);

				//renderer.Draw(va, ib, shader);
				//renderer.Draw(va, 0, 36, shader);


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
	{
		glfwSetWindowShouldClose(window, true);
	}

	float cameraSpeed = 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		//ֻ��Z����ķ���
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

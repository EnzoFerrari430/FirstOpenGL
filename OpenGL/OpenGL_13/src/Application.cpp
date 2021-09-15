/*
p13:
	��ʼ��װ��
	��װ vertex buffer object ��vertex index buffer
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


#pragma region glfw�� ������
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
			//λ��		  //��������
			-0.5f, -0.5f, 0.0f, 0.0f,// 0
			 0.5f, -0.5f, 1.0f, 0.0f,// 1
			 0.5f,  0.5f, 1.0f, 1.0f,// 2
			-0.5f,  0.5f, 0.0f, 1.0f// 3
		};

		//ָʾ�����4������������ν��л���
		unsigned int indices[] = {
			0, 1, 2, //��һ�������ε�3����������
			2, 3, 0  //�ڶ��������ε�3����������
		};

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		VertexArray va;
		VertexBuffer vb(positions, 4 * 4 * sizeof(float));
		VertexBufferLayout layout;
		/*
		����ж�����ԣ�ȷ����AddBuffer֮ǰ��push��ȥ
		��AddBuffer��ʱ������Ҫ����stride
		*/
		layout.Push<float>(2);
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);

		IndexBuffer ib(indices, 6);

		Shader shader("res/shaders/Basic.shader");
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

		//Ҫ��֤�����SetUniform1i���õ�0������󶨵ļ�������Ԫһ��
		//����u_Texture��ʾ��������������ܺ���������б����ͼƬ��Ϣ����һ��
		Texture texture("res/textures/person.png");
		texture.Bind();//��Ҫ����Ԫ0��
		shader.SetUniform1i("u_Texture", 0);

		//���,����Ⱦ��ʱ���ٰ���
		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();

		//����һ��Renderer��
		Renderer renderer;

		//��̬�仯��ɫ
		float r = 0.0f;
		float increment = 0.05f;//����

		while (!glfwWindowShouldClose(window))
		{
			renderer.Clear();

			//ʹ�ö������ݶ���
			shader.Bind();
			shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
			
			renderer.Draw(va, ib, shader);

			//p16:unBind���Ǳ�Ҫ�Ĳ��������ٵ���ʱ�����ã����ǻ�Ӱ������
			//va.Unbind();
			//ib.Unbind();
			//shader.Unbind();

			if (r > 1.0f)
				increment = -0.05f;
			else if (r < 0.0f)
				increment = 0.05f;
			r += increment;

			glfwSwapBuffers(window);

			glfwPollEvents();
		}

		//�뿪������  ���� shader va vb ib
	}
	glfwTerminate();
	//ɾ��glfw������֮��glGetError�᷵�ش���
	//ÿ�ε������glGetError�����ش������������ѭ��
	//����ǳ����˲�������ԭ��(���������ﲡ!!!  �����Ϊʲô...��ԭ��)
	return 0;
}
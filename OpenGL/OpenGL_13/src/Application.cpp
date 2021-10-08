/*
p13:
	��ʼ��װ��
	��װ vertex buffer object ��vertex index buffer

p18:
	blend ���
	��Ͼ��ǽ����ǵ�Դɫ�����ǵ�Ŀ��ɫ��ĳ�ֻ��������Ч�ļ���
	��ϲ���ֻ����RGBAģʽ�½���
	��ϵĹؼ����õ�aֵ�Ͻ�Դɫ��Ŀ��ɫ���ո����ı��ʽ��л�ϣ��Դﵽ��ͬ�̶ȵ�͸���ȡ�
	aֵΪ0����ȫ͸�� aֵΪ1����ȫ��͸����

	Դɫ source color��fragment shader�������������ɫ
	Ŀ��ɫ target color: fragment shader������������ɫ

	Ĭ�ϻ��ֵ�� Դɫ1�� GL_ONE Ŀ��ɫ0 GL_ZERO

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);// ���ʹ�õ�
	
	����:
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

#pragma region glfw�� ������
void glfwErrorHandle(int errCode, const char* errMsg)
{
	std::cout << "[glfw error] (" << errCode << ") :" << errMsg << std::endl;
}
#pragma endregion

#pragma region ��ӡ����
//��ӡ��������
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

	//ʹ��OpenGL����ģʽ
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//Ĭ�����е��Ǽ���ģʽ

	window = glfwCreateWindow(900, 540, "EnzoWindow", NULL, NULL);
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
			//-0.5f, -0.5f, 0.0f, 0.0f,// 0
			// 0.5f, -0.5f, 1.0f, 0.0f,// 1
			// 0.5f,  0.5f, 1.0f, 1.0f,// 2
			//-0.5f,  0.5f, 0.0f, 1.0f, // 3
			100.0f, 100.0f, 0.0f, 0.0f,// 0
			200.0f, 100.0f, 1.0f, 0.0f,// 1
			200.0f, 200.0f, 1.0f, 1.0f,// 2
			100.0f, 200.0f, 0.0f, 1.0f // 3
		};

		//ָʾ�����4������������ν��л���
		unsigned int indices[] = {
			0, 1, 2, //��һ�������ε�3����������
			2, 3, 0  //�ڶ��������ε�3����������
		};

		//blend���
		//������û�ϣ����Կ����һ�����ı任
		//GLCall(glEnable(GL_BLEND));
		//GLCall(glBlendFunc(GL_ONE, GL_ZERO));
		//GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

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

		//һ����������ž���
		//�ȿ��������׼�����꣬Ҳ����������Ļ����
		//���Ը��ݴ�������� ����ֵ����һ�����ű���
		glm::mat4 proj = glm::ortho(0.0f, 900.0f, 0.0f, 540.0f, -1.0f, 1.0f);
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-100.0f, 0.0f, 0.0f));//ƽ�ƾ���
		//glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(200.0f, 200.0f, 0.0f));//ƽ�ƾ���

		//printMatrix4(proj);
		//printMatrix4(view);
		//printMatrix4(model);

		//glm::mat4 mvp = proj * view * model; //����ѡ�񽻸�CPU����Ҳ����ͨ��uniform����GPU����
		//printMatrix4(mvp);

		Shader shader("res/shaders/Basic.shader");
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);
		//shader.SetUniformMat4f("u_MVP", mvp);
		//shader.SetUniformMat4f("u_Model", model);
		//shader.SetUniformMat4f("u_View", view);
		//shader.SetUniformMat4f("u_Proj", proj);

		//Ҫ��֤�����SetUniform1i���õ�0������󶨵ļ�������Ԫһ��
		//����u_Texture��ʾ��������������ܺ���������б����ͼƬ��Ϣ����һ��
		Texture texture("res/textures/MyLogo.png");
		texture.Bind();//��Ҫ����Ԫ0��
		shader.SetUniform1i("u_Texture", 0);

		//���,����Ⱦ��ʱ���ٰ���
		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();

		//����һ��Renderer��
		Renderer renderer;


		//imgui
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init();

		glm::vec3 translation(200.0f, 200.0f, 0.0f);

		//��̬�仯��ɫ
		float r = 0.0f;
		float increment = 0.05f;//����

		while (!glfwWindowShouldClose(window))
		{
			renderer.setBGColor(0.2f, 0.3f, 0.3f, 1.0f);
			renderer.Clear();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
			glm::mat4 mvp = proj * view * model;

			//ʹ�ö������ݶ���
			shader.Bind();
			shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
			shader.SetUniformMat4f("u_MVP", mvp);

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
		//�뿪������  ���� shader va vb ib
	}



	glfwTerminate();
	//ɾ��glfw������֮��glGetError�᷵�ش���
	//ÿ�ε������glGetError�����ش������������ѭ��
	//����ǳ����˲�������ԭ��(���������ﲡ!!!  �����Ϊʲô...��ԭ��)
	return 0;
}
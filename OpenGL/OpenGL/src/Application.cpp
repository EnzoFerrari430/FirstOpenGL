#include <GL/glew.h>
#include <GLFW/glfw3.h>
//#include <GL/glew.h>//#error:  gl.h included before glew.h  gl.h������ GLFW/glfw3.h ��  ����Ҫ��glew.h����������

#include <iostream>

/*
https://www.bilibili.com/video/BV1MJ411u7Bc


02:
legacy opengl:
	���ڵ�openglʹ��������Ⱦģʽ���̶���Ⱦ���ߣ�  ���㵫�����
modern opengl:
	�ִ�openglʹ�ú���ģʽ��core-profile��  ���õ�����Ժ�Ч�ʣ����Ǹ���ѧ
	

GLFW:
	��һ��ר�����OpenGL��C���Կ⣬�����û�����OpenGL�����ģ����崰�ڲ����Լ������û�������



03:
΢���Դ���gl��ֻ֧�ֵ�opengl1.1�汾��Ҫʹ���µ�opengl��ʹ��opengl�ĸ߼����ԣ��ͱ����������µ���չ
���Ҳ�ͬ���Կ���˾Ҳ�ᷢ��һЩֻ���Լ��Կ���֧�ֵ���չ�����������ֶ�ȥ�Կ�����dll���ػ��Եú��鷳
glew��չ����Խ����Щ���⣬��ΪGLEW���Զ�ʶ�����ƽ̨֧�ֵ�ȫ��OpenGL�߼���չ����
glew:
	OpenGL extension weangler

*/
int main(void)
{
	GLFWwindow* window;

	//1.��ʼ��glfw��
	if (!glfwInit())
		return -1;

	//��ҪԤ���� GLEW_STATIC
	//glewInit���ܷ�������
	//First you need to create a valid OpenGL rendering context and call glewInit() to initialize the extension entry points
	//if (glewInit() != GLEW_OK)
	//{
	//	std::cout << "error" << std::endl;
	//}


	//2����һ��window
	//����һ�������Լ�������opengl��opengl es����(OpenGL or OpenGL ES context)
	//�󲿷ֵĿ��ƴ��ڼ��价��ѡ���ͨ��������ʾ��window hints��ָ����
	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		//��ֹ GLFW��ĵ��ã����������б����Ĵ��ں͹�ꡣ
		//�޸����б��޸ĵ�٤���ߣ�gamma ramps���ͷ����з������Դ
		glfwTerminate();
		return -1;
	}

	//3��ָ���Ĵ�������Ϊ��ǰ�����̵߳�������
	//˵���˾��ǶԵ�ǰ���ڵ���OpenGL OpenGL ES  API
	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		std::cout << "error" << std::endl;
	}

	//��ȡ����OpenGL�汾
	std::cout << glGetString(GL_VERSION) << std::endl;

	//4ѭ���жϵ�ǰ�����Ƿ���Ҫ�ر�
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		//΢��ʵ�ֵ��Դ���gl��
		//4.1�������
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);


		//legacy opengl ����opengl��ʽ��һ��������
		glBegin(GL_TRIANGLES);
		//ȷ��3������
		glVertex2f(-0.5f, -0.5f);
		glVertex2f( 0.0f,  0.5f);
		glVertex2f( 0.5f, -0.5f);
		glEnd();

		//4.2����ǰ�󻺳�:
		//˫���� ǰ���壺��Ļ����ʾ������  �󻺳壺��һ֡������
		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		//4.3�¼�����  ��ѯ����
		//ͨ���ڻ�����������ûִ֡��һ��
		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
#include <GLFW/glfw3.h>
/*
https://www.bilibili.com/video/BV1MJ411u7Bc


legacy opengl:
	���ڵ�openglʹ��������Ⱦģʽ���̶���Ⱦ���ߣ�  ���㵫�����
modern opengl:
	�ִ�openglʹ�ú���ģʽ��core-profile��  ���õ�����Ժ�Ч�ʣ����Ǹ���ѧ
	
*/
int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

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

	//�ú����ڵ��õ��߳������õ�ǰָ�����ڵ�OpenGL��OpenGL ES������
	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	//�жϵ�ǰ���ڵĹرձ�־�Ƿ�Ϊtrue
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);


		//legacy opengl ����opengl��ʽ��һ��������
		glBegin(GL_TRIANGLES);
		//ȷ��3������
		glVertex2f(-0.5f, -0.5f);
		glVertex2f(0.0f, 0.5f);
		glVertex2f(0.5f, -0.5f);
		glEnd();

		//ʹ��OpenGL����OpenGL ES������Ⱦʱ���ú�������ָ�����ڵ�ǰ�󻺳���
		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		//�ú���ֻ�����Ѿ����¼������е��¼���Ȼ����������
		//�����¼��ᵼ������Щ�¼���صĴ��ں�����ص�����������
		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
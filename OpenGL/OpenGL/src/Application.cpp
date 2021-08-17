#include <GLFW/glfw3.h>
/*
https://www.bilibili.com/video/BV1MJ411u7Bc


legacy opengl:
	早期的opengl使用立即渲染模式（固定渲染管线）  方便但不灵活
modern opengl:
	现代opengl使用核心模式（core-profile）  更好的灵活性和效率，但是更难学
	
*/
int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	//创建一个窗口以及关联的opengl或opengl es环境(OpenGL or OpenGL ES context)
	//大部分的控制窗口及其环境选项都是通过窗口提示（window hints）指定的
	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		//终止 GLFW库的调用，并销毁所有保留的窗口和光标。
		//修复所有被修改的伽玛线（gamma ramps）释放所有分配的资源
		glfwTerminate();
		return -1;
	}

	//该函数在调用的线程上设置当前指定窗口的OpenGL或OpenGL ES上下文
	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	//判断当前窗口的关闭标志是否为true
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);


		//legacy opengl 早期opengl方式画一个三角形
		glBegin(GL_TRIANGLES);
		//确定3个顶点
		glVertex2f(-0.5f, -0.5f);
		glVertex2f(0.0f, 0.5f);
		glVertex2f(0.5f, -0.5f);
		glEnd();

		//使用OpenGL或者OpenGL ES进行渲染时，该函数交换指定窗口的前后缓冲区
		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		//该函数只处理已经在事件队列中的事件，然后立即返回
		//处理事件会导致与这些事件相关的窗口和输入回调函数被调用
		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
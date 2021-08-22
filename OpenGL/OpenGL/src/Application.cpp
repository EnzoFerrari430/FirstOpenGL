#include <GL/glew.h>
#include <GLFW/glfw3.h>
//#include <GL/glew.h>//#error:  gl.h included before glew.h  gl.h存在在 GLFW/glfw3.h 中  所以要将glew.h放在最上面

#include <iostream>

/*
https://www.bilibili.com/video/BV1MJ411u7Bc


02:
legacy opengl:
	早期的opengl使用立即渲染模式（固定渲染管线）  方便但不灵活
modern opengl:
	现代opengl使用核心模式（core-profile）  更好的灵活性和效率，但是更难学
	

GLFW:
	是一个专门针对OpenGL的C语言库，允许用户创建OpenGL上下文，定义窗口参数以及处理用户的输入



03:
微软自带的gl库只支持到opengl1.1版本，要使用新的opengl，使用opengl的高级特性，就必须下载最新的扩展
而且不同的显卡公司也会发布一些只有自家显卡才支持的扩展函数。所以手动去显卡驱动dll加载会显得很麻烦
glew扩展库可以解决这些问题，因为GLEW能自动识别你的平台支持的全部OpenGL高级扩展函数
glew:
	OpenGL extension weangler

*/
int main(void)
{
	GLFWwindow* window;

	//1.初始化glfw库
	if (!glfwInit())
		return -1;

	//需要预定义 GLEW_STATIC
	//glewInit不能放在这里
	//First you need to create a valid OpenGL rendering context and call glewInit() to initialize the extension entry points
	//if (glewInit() != GLEW_OK)
	//{
	//	std::cout << "error" << std::endl;
	//}


	//2创建一个window
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

	//3把指定的窗口设置为当前调用线程的上下文
	//说白了就是对当前窗口调用OpenGL OpenGL ES  API
	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		std::cout << "error" << std::endl;
	}

	//获取现在OpenGL版本
	std::cout << glGetString(GL_VERSION) << std::endl;

	//4循环判断当前窗口是否需要关闭
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		//微软实现的自带的gl库
		//4.1清除缓冲
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);


		//legacy opengl 早期opengl方式画一个三角形
		glBegin(GL_TRIANGLES);
		//确定3个顶点
		glVertex2f(-0.5f, -0.5f);
		glVertex2f( 0.0f,  0.5f);
		glVertex2f( 0.5f, -0.5f);
		glEnd();

		//4.2交换前后缓冲:
		//双缓冲 前缓冲：屏幕上显示的内容  后缓冲：下一帧的内容
		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		//4.3事件处理  轮询方法
		//通常在缓冲区交换后没帧执行一次
		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
/*
本节使用现代OpenGL方法绘制一个三角形


OpenGL实际操作起来就是一个state machine状态机
先设置一系列状态，然后告诉OpenGL给我绘制一个图像

简单的例子：

告诉OpenGL选择一个buffer
告诉OpenGL选择一个shader
然后给我画一个三角形


绘制流程记为X、

shader（着色器）是运行在GPU上的小程序
这些小程序为图形渲染管线的某个特定部分而运行

*/

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>


/*************************************************
Function:		生成一个shader对象

Description:    生成一个shader对象

Input:          type：指定要生成的shader类型
				source：指定shader对象要装载的源码

Output:			无

Return:         生成的shader对象的id

Date：			2021-8-22

Author：			ENZO
*************************************************/
static unsigned int compileShader(unsigned int type, const std::string& source)
{
	//1.创建一个shader对象
	unsigned int id = glCreateShader(type);

	//2.设置shader的sourceCode
	/*
	将GLSL源码设置到shader中，任何先前保存在shader对象中的源码都将被替换
	如果源码字符串以NULL结尾，则长度参数可以设置为NULL
	*/
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	//3.编译保存在shader中的sourceCode
	glCompileShader(id);

	//TODO: ERROR handling
	//4.查看着色器编译状态是否成功
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (GL_FALSE == result)
	{
		//查看错误原因
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		//alloca：分配的内存在局部函数的上下文中，当调用的函数返回的时候，会自动释放
		char *message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") \
			<< " shader!" << std::endl;
		std::cout << message << std::endl;

		//删除shader
		glDeleteShader(id);
		return 0;

	}

	return id;
}

static unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	//1.创建一个空的program对象
	unsigned int program = glCreateProgram();

	//2.创建shader对象
	unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

	//3.将shader对象链接到program对象中
	glAttachShader(program, vs);
	glAttachShader(program, fs);

	//4.创建一个可执行的program
	/*
	If any shader objects of type GL_VERTEX_SHADER are attached to program, 
	they will be used to create an executable that will run on the programmable vertex processor. 
	If any shader objects of type GL_GEOMETRY_SHADER are attached to program, 
	they will be used to create an executable that will run on the programmable geometry processor. 
	If any shader objects of type GL_FRAGMENT_SHADER are attached to program, 
	they will be used to create an executable that will run on the programmable fragment processor.
	*/
	glLinkProgram(program);

	//5.检查program对象中的可执行文件(Shader)能否在当前的OpenGL环境下执行
	glValidateProgram(program);

	//6.删除中间生成的vertexShader和fragmentShader
	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

int main()
{
	GLFWwindow* window;

	if (!glfwInit())
	{
		return -1;
	}

	window = glfwCreateWindow(640, 480, "EnzoWindow", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		std::cout << "error" << std::endl;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;


	//X:顶点数据
	float positions[6] = {
		-0.5f, -0.5f,
		 0.0f,  0.5f,
		 0.5f, -0.5f,
	};
	//X:1.创建一个buffer 大小为1 id保存在变量buffer中
	unsigned int buffer;//buffer ID
	//生成缓冲对象的数量，保存到一个数组中，这里只生成一个对象，就没有用数组保存了
	glGenBuffers(1, &buffer);
	//X:2.选择buffer,绑定到OpenGL上下文环境中
	//将buffer激活为顶点缓存对象
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	//X:3.将数据拷贝到buffer中
	//GL_STATIC_DRAW,存储内容修改一次并使用多次
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);
	//以上X1-X3 我们就已经把顶点数据存在显卡的内存中了。

	//下面还需要创建顶点和着色器来真正处理这些数据
	//X:~4.启用顶点属性
	/*
	默认情况下，处于性能考虑，所有的顶点着色器的属性变量都是关闭的
	这意味着数据在着色器端是不可见的，哪怕数据已经上传到GPU
	由glEnableVertexAttribArray启用指定属性，才可在顶点着色器中访问逐顶点的属性数据
	*/
	glEnableVertexAttribArray(0);

	//X:4.设置顶点属性
	/*
	void glVertexAttribPointer(	GLuint index,
								GLint size,
								GLenum type,
								GLboolean normalized,
								GLsizei stride,
								const GLvoid * pointer);
	index:需要修改的顶点的属性索引值
			****顶点有很多的属性：坐标，纹理，颜色，等等
				顶点的布局（layout）有关，在布局上，可以把坐标属性放在index = 0的位置，
				纹理属性放在index = 1的位置上，颜色放在index = 2的位置上
	size：每个顶点属性组建的数量. must be 1 2 3 4
			****这里是2维坐标，两个float表示x，y所以这里填2
	type: 每个顶点属性组建的数据类型
	normalized:	数据是否需要标准化
			****有的数据例如颜色，可能是0-255之前的数据，但是OpenGL都是0-1之间的数据，可能需要用到标准化
				但是这里是表示坐标，已经在0-1之间了 不需要标准化。
	stride: 步幅
			****顶点到顶点之间的数据长度
	pointer: 一个顶点中属性的偏移位置
			****假设一个顶点有一个坐标属性，一个纹理属性，一个颜色属性
				坐标属性两个float 纹理属性两个float 颜色属性2两个float
				那么坐标属性的偏移位置就是0  纹理属性偏移位置就是sizeof(float) * 2  颜色属性偏移位置就是sizeof(float) * 4
	*/
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	//X:5生成shader
	//X:5.1  顶点着色器源码
	std::string vertexShader =
		"#version 330 core\n"
		"\n"
		"layout(location = 0) in vec2 position;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	gl_Position = vec4(position.x, position.y, 0.0, 1.0);\n"
		"}\n";

	//"layout(location = 0) out vec4 color;\n"
	// TODO: 这个颜色为啥还是白色的！！！！！！！
	std::string fragmentShader =
		"#version 330 core\n"
		"\n"
		"out vec4 color;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	color = vec4(0.5f, 0.0f, 0.0f, 1.0f);\n"
		"}\n";
	//X:5.2生成shader
	unsigned int shader = createShader(vertexShader, fragmentShader);

	//X:5.3使用program对象作为当前渲染的一部分
	glUseProgram(shader);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);


		//在glClear和glfwSwapBuffers之间告知OpenGL状态机要绘制的图形
		glDrawArrays(GL_TRIANGLES, 0, 3);




		glfwSwapBuffers(window);


		glfwPollEvents();
	}

	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}
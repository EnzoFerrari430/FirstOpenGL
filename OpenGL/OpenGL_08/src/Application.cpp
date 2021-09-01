/*
p08:
	本节我们从一个file中读取2个着色器(shader) 顶点着色器和片段着色器


p09:
	使用index buffer创建一个长方形

	单纯的使用vertex buffer object需要保存6个顶点数据
	会产生重复的2个顶点数据

	index buffer可以指定三角形的顶点索引


*/

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

#include <fstream>//读取文件
#include <sstream>//字符串流

struct shaderProgramSource
{
	std::string vertexSource;
	std::string fragmentSource;
};

//因为这里只是一个简单OpenGL教程，所以使用C++风格读取文件
//C++风格读取文件会比C风格的API慢一些
static shaderProgramSource parseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class shaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];//2个字符串流 一个用来保存vertexShader 一个用来保存fragmentShader
	shaderType type = shaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = shaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = shaderType::FRAGMENT;
		}
		else
		{
			// TODO: type == shaderType::NONE就凉了
			ss[(int)type] << line << "\n";
		}
	}
	return { ss[0].str(), ss[1].str() };
}

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

static int createShader(const std::string& vertexShader, const std::string& fragmentShader)
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
	float positions[] = {
		-0.5f, -0.5f, // 0
		 0.5f, -0.5f, // 1
		 0.5f,  0.5f, // 2
		-0.5f,  0.5f  // 3
	};

	//指示上面的4个顶点数据如何进行绘制
	unsigned int indices[] = {
		0, 1, 2, //第一个三角形的3个顶点索引
		2, 3, 0  //第二个三角形的3个顶点索引
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
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), positions, GL_STATIC_DRAW);
	//以上X1-X3 我们就已经把顶点数据存在显卡的内存中了。

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	//创建index buffer
	unsigned int ibo; //需要是unsigned
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indices, GL_STATIC_DRAW);
	


	//X:5生成shader
	//X:5.1  顶点着色器源码
	shaderProgramSource source = parseShader("res/shaders/Basic.shader");
	std::cout << "VERTEX" << std::endl;
	std::cout << source.vertexSource << std::endl;
	std::cout << "FRAGMENT" << std::endl;
	std::cout << source.fragmentSource << std::endl;

	//X:5.2生成shader
	unsigned int shader = createShader(source.vertexSource, source.fragmentSource);

	//X:5.3使用program对象作为当前渲染的一部分
	glUseProgram(shader);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);


		//在glClear和glfwSwapBuffers之间告知OpenGL状态机要绘制的图形
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		glfwSwapBuffers(window);


		glfwPollEvents();
	}

	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}
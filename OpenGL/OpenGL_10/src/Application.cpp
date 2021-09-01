/*
p10:
	本节讨论如何排查OpenGL调用异常问题

	OpenGL最令人沮丧的就是我明明做了很多操作，但是最后输出的就是一个黑色的框框，也不知道错在哪里。

	本节封装GLGetError来排查调用异常问题

*/

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

#include <fstream>//读取文件
#include <sstream>//字符串流

/*
OpenGL Error存储的一个队列中
glGetError获取一个错误信息，并移除队列
所以 不定期检测会导致不知道某个函数是否调用错误，及ErrorCode
*/

#define ASSERT(x) if (!(x)) __debugbreak()
//缺陷：要考虑作用域的问题
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" << error << "): " << function << 
			" " << file << ":" <<  line << std::endl;
		return false;
	}
	return true;
}

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
	GLCall(unsigned int id = glCreateShader(type));

	const char* src = source.c_str();
	GLCall(glShaderSource(id, 1, &src, nullptr));
	GLCall(glCompileShader(id));

	//TODO: ERROR handling
	//4.查看着色器编译状态是否成功
	int result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	if (GL_FALSE == result)
	{
		//查看错误原因
		int length;
		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		//alloca：分配的内存在局部函数的上下文中，当调用的函数返回的时候，会自动释放
		char *message = (char*)alloca(length * sizeof(char));
		GLCall(glGetShaderInfoLog(id, length, &length, message));
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") \
			<< " shader!" << std::endl;
		std::cout << message << std::endl;

		GLCall(glDeleteShader(id));
		return 0;

	}

	return id;
}

static int createShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	GLCall(unsigned int program = glCreateProgram());

	unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	//6.删除中间生成的vertexShader和fragmentShader
	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));

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

	//必须要在glew库初始化之后才可以调用glGetError
	if (glewInit() != GLEW_OK)
	{
		std::cout << "error" << std::endl;
	}

	GLCall(std::cout << glGetString(GL_VERSION) << std::endl);


	//顶点数据
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

	unsigned int buffer;
	GLCall(glGenBuffers(1, &buffer));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
	GLCall(glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), positions, GL_STATIC_DRAW));
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indices, GL_STATIC_DRAW);



	//X:5生成shader
	//X:5.1  顶点着色器源码
	shaderProgramSource source = parseShader("res/shaders/Basic.shader");

	unsigned int shader = createShader(source.vertexSource, source.fragmentSource);

	glUseProgram(shader);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);


		//在glClear和glfwSwapBuffers之间告知OpenGL状态机要绘制的图形
		//GLClearError();
		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
		//ASSERT(GLLogCall());

		glfwSwapBuffers(window);


		glfwPollEvents();
	}

	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}
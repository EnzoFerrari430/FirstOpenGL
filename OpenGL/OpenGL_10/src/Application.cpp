/*
p10:
	本节讨论如何排查OpenGL调用异常问题

	OpenGL最令人沮丧的就是我明明做了很多操作，但是最后输出的就是一个黑色的框框，也不知道错在哪里。

	本节封装GLGetError来排查调用异常问题

p11:
	GLSL:uniform
	uniform是一种从CPU中的应用向GPU中的着色器发送数据的方式，但是和顶点属性有些不同。
	首先，uniform是全局的(GLobal)。全局表示uniform变量必须在每个着色器程序对象(glUseProgram函数中的着色器对象)
	而且，它uniform可以被着色器程序的任意着色器在任意阶段访问
	再次，无论uniform值设置成什么，uniform会一直保存它们的数据，直到它们被重置或更新


p12:
	使用Vertex Array Object 顶点数组对象

	本质上，我们绑定的缓冲数据，VBO 会保存在一个顶点数组对象中

	之前一直没有提及是因为我们使用的是OpenGL的兼容模式
	OpenGL的兼容模式，为我们创建了一个默认的顶点数组对象，  我们绑定的VBO 绑定在了这个默认的顶点数组对象中

	OpenGL的核心模式是没有默认的顶点数组对象


	顶点数组对象保存以下东西：
	glEnableVertexAttribArray和glDisableVertexAttribArray的调用。
	通过glVertexAttribPointer设置的顶点属性配置。
	通过glVertexAttribPointer调用进行的顶点缓冲对象与顶点属性链接。
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

#pragma region 错误处理
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
#pragma endregion


#pragma region glfw库 错误处理
void glfwErrorHandle(int errCode, const char* errMsg)
{
	std::cout << "[glfw error] (" << errCode << ") :" << errMsg << std::endl;
}
#pragma endregion


#pragma region 解析着色器程序
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
	if (true)
	{

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
#pragma endregion

int main()
{
	GLFWwindow* window;

	glfwSetErrorCallback(glfwErrorHandle);

	if (!glfwInit())
	{
		return -1;
	}

	//使用OpenGL核心模式
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//默认运行的是兼容模式

	window = glfwCreateWindow(640, 480, "EnzoWindow", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);

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


	//生成顶点数组对象
	unsigned int vao;
	GLCall(glGenVertexArrays(1, &vao));
	GLCall(glBindVertexArray(vao));//绑定VAO


	unsigned int buffer;
	GLCall(glGenBuffers(1, &buffer));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
	GLCall(glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), positions, GL_STATIC_DRAW));
	
	GLCall(glEnableVertexAttribArray(0));
	//核心模式下，在没有顶点数组对象（Vertex arrays object）的情况下 glVertexAttribPointer会报错 错误码1282
	//实际上，vertex buffer 和 vertex buffer的layout都是绑定在一个顶点数组对象中
	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

	unsigned int ibo;
	GLCall(glGenBuffers(1, &ibo));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indices, GL_STATIC_DRAW));



	//X:5生成shader
	//X:5.1  顶点着色器源码
	shaderProgramSource source = parseShader("res/shaders/Basic.shader");

	unsigned int shader = createShader(source.vertexSource, source.fragmentSource);

	GLCall(glUseProgram(shader));

	///////////////////////////////////////////////////
	//p11:在程序使用着色器对象之后  可以调用uniform设置颜色

	//1.找到uniform ID,失败返回-1
	GLCall(int location = glGetUniformLocation(shader, "u_Color"));
	ASSERT(location != -1);
	///////////////////////////////////////////////////

	//解绑,在渲染的时候再绑定上
	GLCall(glUseProgram(0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	GLCall(glBindVertexArray(0));

	//动态变化颜色
	float r = 0.0f;
	float increment = 0.05f;//步长

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		/*
		//不使用顶点数组对象（vertex array object）的话
		GLCall(glUseProgram(shader));
		GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
		GLCall(glEnableVertexAttribArray(0));
		GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
		*/

		
		//使用顶点数据对象
		GLCall(glUseProgram(shader));
		GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));
		GLCall(glBindVertexArray(vao));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));

		//在glClear和glfwSwapBuffers之间告知OpenGL状态机要绘制的图形
		//glDrawElements偏移量是字节数 单位是byte所以下一个索引的步长是sizeof(unsigned int)
		//GLCall(glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (const void*)(sizeof(unsigned int) * 3)));
		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

		GLCall(glBindVertexArray(0));


		if (r > 1.0f)
			increment = -0.05f;
		else if (r < 0.0f)
			increment = 0.05f;
		r += increment;

		glfwSwapBuffers(window);


		glfwPollEvents();
	}

	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}
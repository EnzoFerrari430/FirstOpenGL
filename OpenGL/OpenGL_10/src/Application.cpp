/*
p10:
	������������Ų�OpenGL�����쳣����

	OpenGL�����˾�ɥ�ľ������������˺ܶ�����������������ľ���һ����ɫ�Ŀ��Ҳ��֪���������

	���ڷ�װGLGetError���Ų�����쳣����

p11:
	GLSL:uniform
	uniform��һ�ִ�CPU�е�Ӧ����GPU�е���ɫ���������ݵķ�ʽ�����ǺͶ���������Щ��ͬ��
	���ȣ�uniform��ȫ�ֵ�(GLobal)��ȫ�ֱ�ʾuniform����������ÿ����ɫ���������(glUseProgram�����е���ɫ������)
	���ң���uniform���Ա���ɫ�������������ɫ��������׶η���
	�ٴΣ�����uniformֵ���ó�ʲô��uniform��һֱ�������ǵ����ݣ�ֱ�����Ǳ����û����


p12:
	ʹ��Vertex Array Object �����������

	�����ϣ����ǰ󶨵Ļ������ݣ�VBO �ᱣ����һ���������������

	֮ǰһֱû���ἰ����Ϊ����ʹ�õ���OpenGL�ļ���ģʽ
	OpenGL�ļ���ģʽ��Ϊ���Ǵ�����һ��Ĭ�ϵĶ����������  ���ǰ󶨵�VBO ���������Ĭ�ϵĶ������������

	OpenGL�ĺ���ģʽ��û��Ĭ�ϵĶ����������


	����������󱣴����¶�����
	glEnableVertexAttribArray��glDisableVertexAttribArray�ĵ��á�
	ͨ��glVertexAttribPointer���õĶ����������á�
	ͨ��glVertexAttribPointer���ý��еĶ��㻺������붥���������ӡ�
*/

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

#include <fstream>//��ȡ�ļ�
#include <sstream>//�ַ�����

/*
OpenGL Error�洢��һ��������
glGetError��ȡһ��������Ϣ�����Ƴ�����
���� �����ڼ��ᵼ�²�֪��ĳ�������Ƿ���ô��󣬼�ErrorCode
*/

#pragma region ������
#define ASSERT(x) if (!(x)) __debugbreak()
//ȱ�ݣ�Ҫ���������������
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


#pragma region glfw�� ������
void glfwErrorHandle(int errCode, const char* errMsg)
{
	std::cout << "[glfw error] (" << errCode << ") :" << errMsg << std::endl;
}
#pragma endregion


#pragma region ������ɫ������
struct shaderProgramSource
{
	std::string vertexSource;
	std::string fragmentSource;
};

//��Ϊ����ֻ��һ����OpenGL�̳̣�����ʹ��C++����ȡ�ļ�
//C++����ȡ�ļ����C����API��һЩ
static shaderProgramSource parseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class shaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];//2���ַ����� һ����������vertexShader һ����������fragmentShader
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
			// TODO: type == shaderType::NONE������
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
	//4.�鿴��ɫ������״̬�Ƿ�ɹ�
	int result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	if (GL_FALSE == result)
	{
		//�鿴����ԭ��
		int length;
		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		//alloca��������ڴ��ھֲ��������������У������õĺ������ص�ʱ�򣬻��Զ��ͷ�
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

	//6.ɾ���м����ɵ�vertexShader��fragmentShader
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


	//��������
	float positions[] = {
		-0.5f, -0.5f, // 0
		 0.5f, -0.5f, // 1
		 0.5f,  0.5f, // 2
		-0.5f,  0.5f  // 3
	};

	//ָʾ�����4������������ν��л���
	unsigned int indices[] = {
		0, 1, 2, //��һ�������ε�3����������
		2, 3, 0  //�ڶ��������ε�3����������
	};


	//���ɶ����������
	unsigned int vao;
	GLCall(glGenVertexArrays(1, &vao));
	GLCall(glBindVertexArray(vao));//��VAO


	unsigned int buffer;
	GLCall(glGenBuffers(1, &buffer));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
	GLCall(glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), positions, GL_STATIC_DRAW));
	
	GLCall(glEnableVertexAttribArray(0));
	//����ģʽ�£���û�ж����������Vertex arrays object��������� glVertexAttribPointer�ᱨ�� ������1282
	//ʵ���ϣ�vertex buffer �� vertex buffer��layout���ǰ���һ���������������
	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

	unsigned int ibo;
	GLCall(glGenBuffers(1, &ibo));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indices, GL_STATIC_DRAW));



	//X:5����shader
	//X:5.1  ������ɫ��Դ��
	shaderProgramSource source = parseShader("res/shaders/Basic.shader");

	unsigned int shader = createShader(source.vertexSource, source.fragmentSource);

	GLCall(glUseProgram(shader));

	///////////////////////////////////////////////////
	//p11:�ڳ���ʹ����ɫ������֮��  ���Ե���uniform������ɫ

	//1.�ҵ�uniform ID,ʧ�ܷ���-1
	GLCall(int location = glGetUniformLocation(shader, "u_Color"));
	ASSERT(location != -1);
	///////////////////////////////////////////////////

	//���,����Ⱦ��ʱ���ٰ���
	GLCall(glUseProgram(0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	GLCall(glBindVertexArray(0));

	//��̬�仯��ɫ
	float r = 0.0f;
	float increment = 0.05f;//����

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		/*
		//��ʹ�ö����������vertex array object���Ļ�
		GLCall(glUseProgram(shader));
		GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
		GLCall(glEnableVertexAttribArray(0));
		GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
		*/

		
		//ʹ�ö������ݶ���
		GLCall(glUseProgram(shader));
		GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));
		GLCall(glBindVertexArray(vao));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));

		//��glClear��glfwSwapBuffers֮���֪OpenGL״̬��Ҫ���Ƶ�ͼ��
		//glDrawElementsƫ�������ֽ��� ��λ��byte������һ�������Ĳ�����sizeof(unsigned int)
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
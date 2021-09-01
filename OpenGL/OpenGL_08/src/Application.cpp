/*
p08:
	�������Ǵ�һ��file�ж�ȡ2����ɫ��(shader) ������ɫ����Ƭ����ɫ��


p09:
	ʹ��index buffer����һ��������

	������ʹ��vertex buffer object��Ҫ����6����������
	������ظ���2����������

	index buffer����ָ�������εĶ�������


*/

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

#include <fstream>//��ȡ�ļ�
#include <sstream>//�ַ�����

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
	//1.����һ��shader����
	unsigned int id = glCreateShader(type);

	//2.����shader��sourceCode
	/*
	��GLSLԴ�����õ�shader�У��κ���ǰ������shader�����е�Դ�붼�����滻
	���Դ���ַ�����NULL��β���򳤶Ȳ�����������ΪNULL
	*/
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	//3.���뱣����shader�е�sourceCode
	glCompileShader(id);

	//TODO: ERROR handling
	//4.�鿴��ɫ������״̬�Ƿ�ɹ�
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (GL_FALSE == result)
	{
		//�鿴����ԭ��
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		//alloca��������ڴ��ھֲ��������������У������õĺ������ص�ʱ�򣬻��Զ��ͷ�
		char *message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") \
			<< " shader!" << std::endl;
		std::cout << message << std::endl;

		//ɾ��shader
		glDeleteShader(id);
		return 0;

	}

	return id;
}

static int createShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	//1.����һ���յ�program����
	unsigned int program = glCreateProgram();

	//2.����shader����
	unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

	//3.��shader�������ӵ�program������
	glAttachShader(program, vs);
	glAttachShader(program, fs);

	//4.����һ����ִ�е�program
	/*
	If any shader objects of type GL_VERTEX_SHADER are attached to program,
	they will be used to create an executable that will run on the programmable vertex processor.
	If any shader objects of type GL_GEOMETRY_SHADER are attached to program,
	they will be used to create an executable that will run on the programmable geometry processor.
	If any shader objects of type GL_FRAGMENT_SHADER are attached to program,
	they will be used to create an executable that will run on the programmable fragment processor.
	*/
	glLinkProgram(program);

	//5.���program�����еĿ�ִ���ļ�(Shader)�ܷ��ڵ�ǰ��OpenGL������ִ��
	glValidateProgram(program);

	//6.ɾ���м����ɵ�vertexShader��fragmentShader
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


	//X:��������
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

	//X:1.����һ��buffer ��СΪ1 id�����ڱ���buffer��
	unsigned int buffer;//buffer ID
	//���ɻ����������������浽һ�������У�����ֻ����һ�����󣬾�û�������鱣����
	glGenBuffers(1, &buffer);
	//X:2.ѡ��buffer,�󶨵�OpenGL�����Ļ�����
	//��buffer����Ϊ���㻺�����
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	//X:3.�����ݿ�����buffer��
	//GL_STATIC_DRAW,�洢�����޸�һ�β�ʹ�ö��
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), positions, GL_STATIC_DRAW);
	//����X1-X3 ���Ǿ��Ѿ��Ѷ������ݴ����Կ����ڴ����ˡ�

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	//����index buffer
	unsigned int ibo; //��Ҫ��unsigned
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indices, GL_STATIC_DRAW);
	


	//X:5����shader
	//X:5.1  ������ɫ��Դ��
	shaderProgramSource source = parseShader("res/shaders/Basic.shader");
	std::cout << "VERTEX" << std::endl;
	std::cout << source.vertexSource << std::endl;
	std::cout << "FRAGMENT" << std::endl;
	std::cout << source.fragmentSource << std::endl;

	//X:5.2����shader
	unsigned int shader = createShader(source.vertexSource, source.fragmentSource);

	//X:5.3ʹ��program������Ϊ��ǰ��Ⱦ��һ����
	glUseProgram(shader);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);


		//��glClear��glfwSwapBuffers֮���֪OpenGL״̬��Ҫ���Ƶ�ͼ��
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		glfwSwapBuffers(window);


		glfwPollEvents();
	}

	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}
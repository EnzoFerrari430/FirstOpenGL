/*
����ʹ���ִ�OpenGL��������һ��������


OpenGLʵ�ʲ�����������һ��state machine״̬��
������һϵ��״̬��Ȼ�����OpenGL���һ���һ��ͼ��

�򵥵����ӣ�

����OpenGLѡ��һ��buffer
����OpenGLѡ��һ��shader
Ȼ����һ�һ��������


�������̼�ΪX��

shader����ɫ������������GPU�ϵ�С����
��ЩС����Ϊͼ����Ⱦ���ߵ�ĳ���ض����ֶ�����

*/

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>


/*************************************************
Function:		����һ��shader����

Description:    ����һ��shader����

Input:          type��ָ��Ҫ���ɵ�shader����
				source��ָ��shader����Ҫװ�ص�Դ��

Output:			��

Return:         ���ɵ�shader�����id

Date��			2021-8-22

Author��			ENZO
*************************************************/
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

static unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader)
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
	float positions[6] = {
		-0.5f, -0.5f,
		 0.0f,  0.5f,
		 0.5f, -0.5f,
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
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);
	//����X1-X3 ���Ǿ��Ѿ��Ѷ������ݴ����Կ����ڴ����ˡ�

	//���滹��Ҫ�����������ɫ��������������Щ����
	//X:~4.���ö�������
	/*
	Ĭ������£��������ܿ��ǣ����еĶ�����ɫ�������Ա������ǹرյ�
	����ζ����������ɫ�����ǲ��ɼ��ģ����������Ѿ��ϴ���GPU
	��glEnableVertexAttribArray����ָ�����ԣ��ſ��ڶ�����ɫ���з����𶥵����������
	*/
	glEnableVertexAttribArray(0);

	//X:4.���ö�������
	/*
	void glVertexAttribPointer(	GLuint index,
								GLint size,
								GLenum type,
								GLboolean normalized,
								GLsizei stride,
								const GLvoid * pointer);
	index:��Ҫ�޸ĵĶ������������ֵ
			****�����кܶ�����ԣ����꣬������ɫ���ȵ�
				����Ĳ��֣�layout���йأ��ڲ����ϣ����԰��������Է���index = 0��λ�ã�
				�������Է���index = 1��λ���ϣ���ɫ����index = 2��λ����
	size��ÿ�����������齨������. must be 1 2 3 4
			****������2ά���꣬����float��ʾx��y����������2
	type: ÿ�����������齨����������
	normalized:	�����Ƿ���Ҫ��׼��
			****�е�����������ɫ��������0-255֮ǰ�����ݣ�����OpenGL����0-1֮������ݣ�������Ҫ�õ���׼��
				���������Ǳ�ʾ���꣬�Ѿ���0-1֮���� ����Ҫ��׼����
	stride: ����
			****���㵽����֮������ݳ���
	pointer: һ�����������Ե�ƫ��λ��
			****����һ��������һ���������ԣ�һ���������ԣ�һ����ɫ����
				������������float ������������float ��ɫ����2����float
				��ô�������Ե�ƫ��λ�þ���0  ��������ƫ��λ�þ���sizeof(float) * 2  ��ɫ����ƫ��λ�þ���sizeof(float) * 4
	*/
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	//X:5����shader
	//X:5.1  ������ɫ��Դ��
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
	// TODO: �����ɫΪɶ���ǰ�ɫ�ģ�������������
	std::string fragmentShader =
		"#version 330 core\n"
		"\n"
		"out vec4 color;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	color = vec4(0.5f, 0.0f, 0.0f, 1.0f);\n"
		"}\n";
	//X:5.2����shader
	unsigned int shader = createShader(vertexShader, fragmentShader);

	//X:5.3ʹ��program������Ϊ��ǰ��Ⱦ��һ����
	glUseProgram(shader);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);


		//��glClear��glfwSwapBuffers֮���֪OpenGL״̬��Ҫ���Ƶ�ͼ��
		glDrawArrays(GL_TRIANGLES, 0, 3);




		glfwSwapBuffers(window);


		glfwPollEvents();
	}

	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}
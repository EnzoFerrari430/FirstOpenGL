/*
����ʹ���ִ�OpenGL��������һ��������


OpenGLʵ�ʲ�����������һ��state machine״̬��
������һϵ��״̬��Ȼ�����OpenGL���һ���һ��ͼ��

�򵥵����ӣ�

����OpenGLѡ��һ��buffer
����OpenGLѡ��һ��shader
Ȼ����һ�һ��������


�������̼�ΪX

*/

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

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


	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);


		//��glClear��glfwSwapBuffers֮���֪OpenGL״̬��Ҫ���Ƶ�ͼ��
		glDrawArrays(GL_TRIANGLES, 0, 3);




		glfwSwapBuffers(window);


		glfwPollEvents();
	}



	glfwTerminate();
	return 0;
}
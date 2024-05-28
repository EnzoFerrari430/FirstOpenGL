#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <learnopengl/shader.h>

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

#define QuadsUniformBlockSize 1024

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    // 获取单个UBO大小的最大限制
    GLint maxUniformBlockSize;
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxUniformBlockSize);
    std::cout << "Maximum number of Uniform Block Size: " << maxUniformBlockSize << " (ie:bytes)" << std::endl;

    Shader shader("10.1.instancing.vs", "10.1.instancing.fs");

    float quadVertices[] = {
        // 位置          // 颜色
        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
         0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
        -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
         0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
         0.05f,  0.05f,  0.0f, 1.0f, 1.0f
    };

    unsigned int cubeVAO, VBO;
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void*)(2 * sizeof(float)));

    glBindVertexArray(0);

    // positions
    glm::vec2 translations[QuadsUniformBlockSize];
    int index = 0;
    float offset = 0.1f;
    for (int y = -10; y < 10; y += 2)
    {
        for (int x = -10; x < 10; x += 2)
        {
            glm::vec2 translation;
            translation.x = (float)x / 10.0f + offset;
            translation.y = (float)y / 10.0f + offset;
            translations[index++] = translation;
        }
    }

    shader.use();
    for (unsigned int i = 0; i < 100; i++)
    {
        std::stringstream ss;
        std::string index;
        ss << i;
        index = ss.str();
        shader.setVec2(("offsets[" + index + "]").c_str(), translations[i]);
    }

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(cubeVAO);
        shader.use();
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, QuadsUniformBlockSize);


        glfwPollEvents();
        glfwSwapBuffers(window);
    }


    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &cubeVAO);

    glfwTerminate();
    return 0;
}
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <learnopengl/camera.h>
#include <learnopengl/shader.h>

void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void GPU_select_save_to_image(const char* filePath);
bool image_print_pressed = false;
bool image_print_pressing = false;

const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    Shader shader("1.3.test_tga_file.vs", "1.3.test_tga_file.fs");

    float vertices[] = {
        //position          //color
         0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void*)(3 * sizeof(float)));

    // Éî¶ÈÌùÍ¼

    while (!glfwWindowShouldClose(window))
    {
        // timing
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }


    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !image_print_pressing)
    {
        image_print_pressed = !image_print_pressed;
        image_print_pressing = true;
        GPU_select_save_to_image("aaa.tga");
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE)
    {
        image_print_pressing = false;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{

}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

}

#include "tgaimage.h"
void GPU_select_save_to_image(const char* filePath)
{
    FILE* output_image = NULL;
    int output_width, output_height;

    output_width = SCR_WIDTH;
    output_height = SCR_HEIGHT;

    int bytespp = 4;

    /// READ THE PIXELS VALUES from FBO AND SAVE TO A .PPM FILE
    int x, y, k;
    unsigned char* r_buf = (unsigned char*)malloc(output_width * output_height * 4);

    int color_x = 0;
    int color_y = 0;

    glReadPixels(color_x, color_y, output_width, output_height, GL_RGBA, GL_UNSIGNED_BYTE, r_buf);
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        std::cout << "GL ERROR when glGetTexImage" << std::endl;
    }

    unsigned char developer_area_ref[4] = { 0, 0, 0, 0 };
    unsigned char extension_area_ref[4] = { 0, 0, 0, 0 };
    unsigned char footer[18] = { 'T', 'R','U', 'E', 'V', 'I', 'S', 'I', 'O', 'N','-', 'X', 'F', 'I', 'L', 'E', '.', '\0' };

    std::ofstream out;
    out.open(filePath, std::ios::binary);
    if (!out.is_open())
    {
        std::cerr << "can't open file " << filePath << "\n";
        out.close();
        return;
    }
    TGA_Header header;
    memset(&header, 0, sizeof(TGA_Header));
    header.bitsperpixel = bytespp << 3;
    header.width = output_width;
    header.height = output_height;
    header.datatypecode = 2;
    header.imagedescriptor = 0x20; // top-left origin
    out.write((char*)&header, sizeof(TGA_Header));
    if (!out.good())
    {
        out.close();
        std::cerr << "can't dump the tga file\n";
        return;
    }

    out.write((char*)r_buf, output_width * output_height * bytespp);
    if (!out.good())
    {
        std::cerr << "can't unload raw data\n";
        out.close();
        return;
    }


    out.write((char*)developer_area_ref, sizeof(developer_area_ref));
    if (!out.good())
    {
        std::cerr << "can't dump the tga file\n";
        out.close();
        return;
    }

    out.write((char*)extension_area_ref, sizeof(extension_area_ref));
    if (!out.good())
    {
        std::cerr << "can't dump the tga file\n";
        out.close();
        return;
    }

    out.write((char*)footer, sizeof(footer));
    if (!out.good())
    {
        std::cerr << "can't dump the tga file\n";
        out.close();
        return;
    }
    out.close();
    free(r_buf);
}
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>

#include <learnopengl/camera.h>
#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>


void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(const char* path);

const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = float(SCR_WIDTH) / 2.0f;
float lastY = float(SCR_HEIGHT) / 2.0f;
bool firstMouse = true;
bool cameraViewMove = false;

// Blinn-Phong
bool blinn = false;
bool blinnKeyPressed = false;

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

    glEnable(GL_DEPTH_TEST);

    Shader shader("1.advanced_lighting.vs", "1.advanced_lighting.fs");
    Shader lightShader("1.light.vs", "1.light.fs");

    float lightVertices[] = {
        // positions
        -0.02f, -0.02f, -0.02f,
         0.02f, -0.02f, -0.02f,
         0.02f,  0.02f, -0.02f,
         0.02f,  0.02f, -0.02f,
        -0.02f,  0.02f, -0.02f,
        -0.02f, -0.02f, -0.02f,

        -0.02f, -0.02f,  0.02f,
         0.02f, -0.02f,  0.02f,
         0.02f,  0.02f,  0.02f,
         0.02f,  0.02f,  0.02f,
        -0.02f,  0.02f,  0.02f,
        -0.02f, -0.02f,  0.02f,

        -0.02f,  0.02f,  0.02f,
        -0.02f,  0.02f, -0.02f,
        -0.02f, -0.02f, -0.02f,
        -0.02f, -0.02f, -0.02f,
        -0.02f, -0.02f,  0.02f,
        -0.02f,  0.02f,  0.02f,

         0.02f,  0.02f,  0.02f,
         0.02f,  0.02f, -0.02f,
         0.02f, -0.02f, -0.02f,
         0.02f, -0.02f, -0.02f,
         0.02f, -0.02f,  0.02f,
         0.02f,  0.02f,  0.02f,

        -0.02f, -0.02f, -0.02f,
         0.02f, -0.02f, -0.02f,
         0.02f, -0.02f,  0.02f,
         0.02f, -0.02f,  0.02f,
        -0.02f, -0.02f,  0.02f,
        -0.02f, -0.02f, -0.02f,

        -0.02f,  0.02f, -0.02f,
         0.02f,  0.02f, -0.02f,
         0.02f,  0.02f,  0.02f,
         0.02f,  0.02f,  0.02f,
        -0.02f,  0.02f,  0.02f,
        -0.02f,  0.02f, -0.02f
    };

    unsigned int lightVAO, lightVBO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glGenBuffers(1, &lightVBO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lightVertices), lightVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (const void*)0);

    float planeVertices[] = {
        // positions            // normals         // texcoords
         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
         10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
    };

    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glBindVertexArray(planeVAO);

    glGenBuffers(1, &planeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void*)(6 * sizeof(float)));

    unsigned int floorTexture = loadTexture(FileSystem::getPath("resources/textures/wood.png").c_str());

    shader.use();
    shader.setInt("texture1", 0);

    glm::vec3 lightPos(0.0f, 0.0f, 0.0f);

    while (!glfwWindowShouldClose(window))
    {
        // timing
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), float(SCR_WIDTH) / float(SCR_HEIGHT), 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        glBindVertexArray(lightVAO);
        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);
        lightShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(planeVAO);
        shader.use();

        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("model", model);

        shader.setVec3("lightPos", lightPos);
        shader.setVec3("viewPos", camera.Position);
        shader.setBool("blinn", blinn);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }


    glfwTerminate();
    return -1;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !blinnKeyPressed)
    {
        blinn = !blinn;
        blinnKeyPressed = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
    {
        blinnKeyPressed = false;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        cameraViewMove = true;
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
        cameraViewMove = false;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;

        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    if (cameraViewMove)
        camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(const char* path)
{
    unsigned int texture;
    glGenTextures(1, &texture);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return texture;
}
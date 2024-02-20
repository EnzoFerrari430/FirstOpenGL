#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <learnopengl/camera.h>
#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>

#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define IMGUI_IMPL_API
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(const char* path);

struct DirectionalLight
{
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct PointLight
{
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;
};

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool cameraViewMove = false;
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// ImGui
bool show_demo_window = true;

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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    //
    glEnable(GL_DEPTH_TEST);

    Shader lightingShader("6.multiple_lights.vs", "6.multiple_lights.fs");
    Shader lightCubeShader("6.light_cube.vs", "6.light_cube.fs");

    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    unsigned int VBO, cubeVAO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void*)(6 * sizeof(float)));


    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void*)0);

    glBindVertexArray(0);

    stbi_set_flip_vertically_on_load(true);
    unsigned int diffuseMap = loadTexture(FileSystem::getPath("resources/textures/container2.png").c_str());
    unsigned int specularMap = loadTexture(FileSystem::getPath("resources/textures/container2_specular.png").c_str());

    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);

    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,   0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f,  -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f,  -3.5f),
        glm::vec3(-1.7f,  3.0f,  -7.5f),
        glm::vec3( 1.3f, -2.0f,  -2.5f),
        glm::vec3( 1.5f,  2.0f,  -2.5f),
        glm::vec3( 1.5f,  0.2f,  -1.5f),
        glm::vec3(-1.3f,  1.0f,  -1.5f)
    };

    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,   2.0f),
        glm::vec3( 2.3f, -3.3f,  -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f,  -3.0f)
    };

    glm::vec3 pointLightColors[] = {
        glm::vec3(0.2f, 0.2f, 0.6f),
        glm::vec3(0.3f, 0.3f, 0.7f),
        glm::vec3(0.0f, 0.0f, 0.3f),
        glm::vec3(0.4f, 0.4f, 0.4f)
    };

    DirectionalLight dirLight;
    dirLight.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    dirLight.ambient = glm::vec3(0.05f);
    dirLight.diffuse = glm::vec3(0.4f);
    dirLight.specular = glm::vec3(0.5f);

    PointLight pointLights[4];
    pointLights[0].position = pointLightPositions[0];
    pointLights[0].ambient = pointLightColors[0] * 0.1f;
    pointLights[0].diffuse = pointLightColors[0] * 0.8f;
    pointLights[0].specular = pointLightColors[0];
    pointLights[0].constant = 1.0f;
    pointLights[0].linear = 0.09f;
    pointLights[0].quadratic = 0.032f;

    pointLights[1].position = pointLightPositions[1];
    pointLights[1].ambient = pointLightColors[1] * 0.1f;
    pointLights[1].diffuse = pointLightColors[1] * 0.8f;
    pointLights[1].specular = pointLightColors[1];
    pointLights[1].constant = 1.0f;
    pointLights[1].linear = 0.09f;
    pointLights[1].quadratic = 0.032f;

    pointLights[2].position = pointLightPositions[2];
    pointLights[2].ambient = pointLightColors[2] * 0.1f;
    pointLights[2].diffuse = pointLightColors[2] * 0.8f;
    pointLights[2].specular = pointLightColors[2];
    pointLights[2].constant = 1.0f;
    pointLights[2].linear = 0.09f;
    pointLights[2].quadratic = 0.032f;

    pointLights[3].position = pointLightPositions[3];
    pointLights[3].ambient = pointLightColors[3] * 0.1f;
    pointLights[3].diffuse = pointLightColors[3] * 0.8f;
    pointLights[3].specular = pointLightColors[3];
    pointLights[3].constant = 1.0f;
    pointLights[3].linear = 0.09f;
    pointLights[3].quadratic = 0.032f;

    SpotLight spotLight;
    spotLight.ambient = glm::vec3(0.0f);
    spotLight.diffuse = glm::vec3(0.8f, 0.8f, 0.0f);
    spotLight.specular = glm::vec3(0.8f, 0.8f, 0.0f);
    spotLight.constant = 1.0f;
    spotLight.linear = 0.09f;
    spotLight.quadratic = 0.032f;
    spotLight.cutOff = 12.5f;//角度 传入的时候要转换成cos值
    spotLight.outerCutOff = 17.5f;


    //ImGui
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();


    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Render
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

        lightCubeShader.use();
        glBindVertexArray(lightCubeVAO);
        lightCubeShader.setMat4("view", view);
        lightCubeShader.setMat4("projection", projection);
        for (int i = 0; i < 4; ++i)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            lightCubeShader.setMat4("model", model);
            lightCubeShader.setVec3("color", pointLightColors[i]);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        //lightingShader
        lightingShader.use();
        glBindVertexArray(cubeVAO);

        lightingShader.setMat4("view", view);
        lightingShader.setMat4("projection", projection);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);
        lightingShader.setFloat("material.shininess", 32.0f);
        lightingShader.setVec3("viewPos", camera.Position);

        //directional light
        lightingShader.setVec3("dirLight.direction", dirLight.direction);
        lightingShader.setVec3("dirLight.ambient", dirLight.ambient);
        lightingShader.setVec3("dirLight.diffuse", dirLight.diffuse);
        lightingShader.setVec3("dirLight.specular", dirLight.specular);

        //point light
        lightingShader.setVec3("pointLights[0].position",   pointLights[0].position);
        lightingShader.setVec3("pointLights[0].ambient",    pointLights[0].ambient);
        lightingShader.setVec3("pointLights[0].diffuse",    pointLights[0].diffuse);
        lightingShader.setVec3("pointLights[0].specular",   pointLights[0].specular);
        lightingShader.setFloat("pointLights[0].constant",  pointLights[0].constant);
        lightingShader.setFloat("pointLights[0].linear",    pointLights[0].linear);
        lightingShader.setFloat("pointLights[0].quadratic", pointLights[0].quadratic);

        lightingShader.setVec3("pointLights[1].position",   pointLights[1].position);
        lightingShader.setVec3("pointLights[1].ambient",    pointLights[1].ambient);
        lightingShader.setVec3("pointLights[1].diffuse",    pointLights[1].diffuse);
        lightingShader.setVec3("pointLights[1].specular",   pointLights[1].specular);
        lightingShader.setFloat("pointLights[1].constant",  pointLights[1].constant);
        lightingShader.setFloat("pointLights[1].linear",    pointLights[1].linear);
        lightingShader.setFloat("pointLights[1].quadratic", pointLights[1].quadratic);

        lightingShader.setVec3("pointLights[2].position",   pointLights[2].position);
        lightingShader.setVec3("pointLights[2].ambient",    pointLights[2].ambient);
        lightingShader.setVec3("pointLights[2].diffuse",    pointLights[2].diffuse);
        lightingShader.setVec3("pointLights[2].specular",   pointLights[2].specular);
        lightingShader.setFloat("pointLights[2].constant",  pointLights[2].constant);
        lightingShader.setFloat("pointLights[2].linear",    pointLights[2].linear);
        lightingShader.setFloat("pointLights[2].quadratic", pointLights[2].quadratic);

        lightingShader.setVec3("pointLights[3].position",   pointLights[3].position);
        lightingShader.setVec3("pointLights[3].ambient",    pointLights[3].ambient);
        lightingShader.setVec3("pointLights[3].diffuse",    pointLights[3].diffuse);
        lightingShader.setVec3("pointLights[3].specular",   pointLights[3].specular);
        lightingShader.setFloat("pointLights[3].constant",  pointLights[3].constant);
        lightingShader.setFloat("pointLights[3].linear",    pointLights[3].linear);
        lightingShader.setFloat("pointLights[3].quadratic", pointLights[3].quadratic);

        //spot light
        lightingShader.setVec3("spotLight.position", camera.Position);
        lightingShader.setVec3("spotLight.direction", camera.Front);
        lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(spotLight.cutOff)));
        lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(spotLight.outerCutOff)));
        lightingShader.setVec3("spotLight.ambient", spotLight.ambient);
        lightingShader.setVec3("spotLight.diffuse", spotLight.diffuse);
        lightingShader.setVec3("spotLight.specular", spotLight.specular);
        lightingShader.setFloat("spotLight.constant", spotLight.constant);
        lightingShader.setFloat("spotLight.linear", spotLight.linear);
        lightingShader.setFloat("spotLight.quadratic", spotLight.quadratic);

        for (int i = 0; i < 10; ++i)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            lightingShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        //ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::Begin("Debug");
        ImGui::Text("Directional Light");
        ImGui::SliderFloat3("Directional Light Direction", &dirLight.direction.x, 0.0f, 1.0f);
        ImGui::SliderFloat3("Directional Light Ambient", &dirLight.ambient.x, 0.0f, 1.0f);
        ImGui::SliderFloat3("Directional Light Diffuse", &dirLight.diffuse.x, 0.0f, 1.0f);
        ImGui::SliderFloat3("Directional Light Specular", &dirLight.specular.x, 0.0f, 1.0f);
        ImGui::Text("Point Light0");
        ImGui::SliderFloat3("Point Light0 Ambient",  &pointLights[0].ambient.x, 0.0f, 1.0f);
        ImGui::SliderFloat3("Point Light0 Diffuse",  &pointLights[0].diffuse.x, 0.0f, 1.0f);
        ImGui::SliderFloat3("Point Light0 Specular", &pointLights[0].specular.x, 0.0f, 1.0f);
        ImGui::Text("Point Light1");
        ImGui::SliderFloat3("Point Light1 Ambient",  &pointLights[1].ambient.x, 0.0f, 1.0f);
        ImGui::SliderFloat3("Point Light1 Diffuse",  &pointLights[1].diffuse.x, 0.0f, 1.0f);
        ImGui::SliderFloat3("Point Light1 Specular", &pointLights[1].specular.x, 0.0f, 1.0f);
        ImGui::Text("Point Light2");
        ImGui::SliderFloat3("Point Light2 Ambient",  &pointLights[2].ambient.x, 0.0f, 1.0f);
        ImGui::SliderFloat3("Point Light2 Diffuse",  &pointLights[2].diffuse.x, 0.0f, 1.0f);
        ImGui::SliderFloat3("Point Light2 Specular", &pointLights[2].specular.x, 0.0f, 1.0f);
        ImGui::Text("Point Light3");
        ImGui::SliderFloat3("Point Light3 Ambient",  &pointLights[3].ambient.x, 0.0f, 1.0f);
        ImGui::SliderFloat3("Point Light3 Diffuse",  &pointLights[3].diffuse.x, 0.0f, 1.0f);
        ImGui::SliderFloat3("Point Light3 Specular", &pointLights[3].specular.x, 0.0f, 1.0f);
        ImGui::Text("Spot Light");
        ImGui::SliderFloat3("Spot Ambient", &spotLight.ambient.x, 0.0f, 1.0f);
        ImGui::SliderFloat3("Spot Diffuse", &spotLight.diffuse.x, 0.0f, 1.0f);
        ImGui::SliderFloat3("Spot Specular", &spotLight.specular.x, 0.0f, 1.0f);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteTextures(1, &diffuseMap);
    glDeleteTextures(1, &specularMap);

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();

    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);

    //检测鼠标点击事件
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        cameraViewMove = true;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
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
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        if (nrChannels == 3)
            format = GL_RGB;
        if (nrChannels == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
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
    return textureID;
}

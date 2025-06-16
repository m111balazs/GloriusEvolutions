#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>
#include "Camera.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Initialize camera with proper position above the ground
Camera camera(glm::vec3(0.0f, 2.0f, 5.0f)); // Start at y=2 (above ground) and z=5 (back from origin)

float lastX = 640, lastY = 360;
bool firstMouse = true;
float deltaTime = 0.0f; 
float lastFrame = 0.0f;
bool altHeld = false;

// Remove these redundant variables - we're using the Camera class instead
// glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
// glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
// glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;

unsigned int compileShader(unsigned int type, const std::string& source) {
    unsigned int shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info[512];
        glGetShaderInfoLog(shader, 512, nullptr, info);
        std::cerr << "Shader compile error: " << info << std::endl;
    }

    return shader;
}

unsigned int createShaderProgram(const std::string& vertexSrc, const std::string& fragmentSrc) {
    unsigned int vertex = compileShader(GL_VERTEX_SHADER, vertexSrc);
    unsigned int fragment = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char info[512];
        glGetProgramInfoLog(program, 512, nullptr, info);
        std::cerr << "Shader link error: " << info << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return program;
}

std::string loadFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << path << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    // Skip mouse input if ImGui wants to capture it
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) return;

    // Always update lastX and lastY to avoid jump
    if (firstMouse) {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
        return; // Skip processing on first mouse input
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos; // Reversed Y coordinates to fix inversion

    lastX = (float)xpos;
    lastY = (float)ypos;

    // Prevent camera movement while Alt is held
    if (altHeld) {
        return;
    }

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    // Skip scroll input if ImGui wants to capture it
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) return;

    camera.ProcessMouseScroll((float)yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_LEFT_ALT || key == GLFW_KEY_RIGHT_ALT) {
        if (action == GLFW_PRESS) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            altHeld = true;
        }
        if (action == GLFW_RELEASE) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            altHeld = false;
            firstMouse = true; // Reset to prevent camera jump
        }
    }
}

void processInput(GLFWwindow *window) {
    // Skip keyboard input if ImGui wants to capture it
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard) return;

    // Use deltaTime for smooth movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    
    // Add escape key to close window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);

    std::cout << "DEBUG: Current working directory is:\n" << std::filesystem::current_path() << std::endl;

    GLFWwindow* window = glfwCreateWindow(1280, 720, "GloriousEvolutions", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // Auto-focus the window on startup
    glfwFocusWindow(window);
    glfwRequestWindowAttention(window);
    
    // Small delay to ensure window is ready
    glfwPollEvents();

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Setup callbacks
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    glEnable(GL_DEPTH_TEST);

    // Triangle vertices (position + color)
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
         0.0f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
    };

    // Ground vertices (larger ground plane)
    float groundVertices[] = {
        // positions           // colors (grass green)
        -50.0f, 0.0f,  50.0f,   0.2f, 0.6f, 0.2f,
         50.0f, 0.0f,  50.0f,   0.2f, 0.6f, 0.2f,
         50.0f, 0.0f, -50.0f,   0.2f, 0.6f, 0.2f,

         50.0f, 0.0f, -50.0f,   0.2f, 0.6f, 0.2f,
        -50.0f, 0.0f, -50.0f,   0.2f, 0.6f, 0.2f,
        -50.0f, 0.0f,  50.0f,   0.2f, 0.6f, 0.2f,
    };

    // Triangle VAO/VBO
    unsigned int VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Load or create fallback shaders
    std::string vertexShaderSource = loadFile("../../src/shaders/vertex.glsl");
    std::string fragmentShaderSource = loadFile("../../src/shaders/fragment.glsl");
    
    // Fallback shaders if files don't exist
    if (vertexShaderSource.empty()) {
        vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vertexColor = aColor;
}
)";
    }
    
    if (fragmentShaderSource.empty()) {
        fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 vertexColor;

void main()
{
    FragColor = vec4(vertexColor, 1.0);
}
)";
    }

    unsigned int shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

    // Ground VAO/VBO
    unsigned int groundVAO, groundVBO;
    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);

    glBindVertexArray(groundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    std::cout << "Camera initialized at position: (" << camera.Position.x << ", " << camera.Position.y << ", " << camera.Position.z << ")" << std::endl;
    std::cout << "Controls: WASD to move, mouse to look around, Alt to toggle cursor, scroll to zoom" << std::endl;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Handle Alt key state changes
        bool altNow = glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS || 
                      glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS;

        if (altNow && !altHeld) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            altHeld = true;
        } else if (!altNow && altHeld) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            firstMouse = true;
            // Center cursor to prevent jump
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            glfwSetCursorPos(window, width / 2.0, height / 2.0);
            altHeld = false;
        }

        processInput(window);

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Debug UI
        ImGui::Begin("Debug Info");
        ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
        ImGui::Text("Camera Pos: (%.2f, %.2f, %.2f)", camera.Position.x, camera.Position.y, camera.Position.z);
        ImGui::Text("Camera Front: (%.2f, %.2f, %.2f)", camera.Front.x, camera.Front.y, camera.Front.z);
        ImGui::Text("Camera Yaw: %.1f  Pitch: %.1f", camera.Yaw, camera.Pitch);
        ImGui::Text("Camera Zoom: %.1f", camera.Zoom);
        ImGui::Text("Alt Held: %s", altHeld ? "Yes" : "No");
        ImGui::Text("Delta Time: %.4f", deltaTime);
        
        // Movement controls
        ImGui::SliderFloat("Movement Speed", &camera.MovementSpeed, 0.1f, 10.0f);
        ImGui::SliderFloat("Mouse Sensitivity", &camera.MouseSensitivity, 0.01f, 1.0f);
        
        // Interactive elements
        static float triangleY = 1.0f;
        ImGui::SliderFloat("Triangle Height", &triangleY, 0.0f, 10.0f);
        
        static float clearColor[3] = {0.2f, 0.1f, 0.3f};
        ImGui::ColorEdit3("Background Color", clearColor);
        
        // Reset camera button
        if (ImGui::Button("Reset Camera")) {
            camera.Position = glm::vec3(0.0f, 2.0f, 5.0f);
            camera.Yaw = -90.0f;
            camera.Pitch = 0.0f;
            camera.updateCameraVectors();
        }
        
        ImGui::End();

        // Clear with dynamic background color
        glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Update view/projection matrices
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), 1280.0f / 720.0f, 0.1f, 100.0f);

        int modelLoc = glGetUniformLocation(shaderProgram, "model");
        int viewLoc  = glGetUniformLocation(shaderProgram, "view");
        int projLoc  = glGetUniformLocation(shaderProgram, "projection");

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Draw Ground
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glBindVertexArray(groundVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Draw Triangle with adjustable height
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, triangleY, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &groundVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &groundVBO);
    glDeleteProgram(shaderProgram);
    
    glfwTerminate();
    return 0;
}
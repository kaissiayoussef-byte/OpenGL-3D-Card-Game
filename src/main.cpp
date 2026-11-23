#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 800;

const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform vec3 tintColor;
uniform bool useTexture;

void main() {
    if (useTexture) {
        FragColor = texture(texture1, TexCoord) * vec4(tintColor, 1.0);
    } else {
        FragColor = vec4(tintColor, 1.0);
    }
}
)";

unsigned int compileShader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation error:\n" << infoLog << "\n";
    }
    return shader;
}

unsigned int createShaderProgram() {
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Shader program link error:\n" << infoLog << "\n";
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

unsigned int loadTexture(const std::string& path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);
    int width = 0, height = 0, nrChannels = 0;

    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (!data) {
        std::string altPath = "../" + path;
        data = stbi_load(altPath.c_str(), &width, &height, &nrChannels, 0);
    }

    if (data && width > 0 && height > 0) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        GLenum format = GL_RGB;
        if (nrChannels == 1) format = GL_RED;
        else if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
    else {
        std::cout << "Fallback texture used for: " << path << "\n";
        unsigned char fallback[3] = { 120, 80, 45 };
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, fallback);
    }

    return textureID;
}

unsigned int createCubeVAO() {
    float vertices[] = {
        // Back face
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        // Left face
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        // Right face
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         // Bottom face
         -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
          0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
          0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
          0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         // Top face
         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
          0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
          0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
          0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    return VAO;
}

unsigned int createTornPosterVAO() {
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
         0.2f, -0.5f, 0.0f,  0.7f, 0.0f,
         0.2f,  0.5f, 0.0f,  0.7f, 1.0f,

        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
         0.2f,  0.5f, 0.0f,  0.7f, 1.0f,
        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f,

         0.2f, -0.5f, 0.0f,  0.7f, 0.0f,
         0.5f, -0.2f, 0.0f,  1.0f, 0.3f,
         0.2f,  0.5f, 0.0f,  0.7f, 1.0f
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    return VAO;
}

void drawMesh(unsigned int shader, unsigned int VAO, int vertexCount,
    const glm::mat4& model, const glm::vec3& tint,
    bool useTexture, unsigned int textureID = 0) {
    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3fv(glGetUniformLocation(shader, "tintColor"), 1, glm::value_ptr(tint));
    glUniform1i(glGetUniformLocation(shader, "useTexture"), useTexture ? 1 : 0);

    if (useTexture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUniform1i(glGetUniformLocation(shader, "texture1"), 0);
    }

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Helper for smooth linear interpolation
float lerp(float current, float target, float speed, float dt) {
    float t = speed * dt;
    if (t > 1.0f) t = 1.0f;
    return current + (target - current) * t;
}

int main() {
    std::srand((unsigned int)std::time(nullptr));


    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Card Game Project", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    unsigned int shaderProgram = createShaderProgram();
    unsigned int cubeVAO = createCubeVAO();
    unsigned int posterVAO = createTornPosterVAO();

    // Load textures (replace filenames with your team's assets)
    unsigned int tableTex = loadTexture("assets/table.jfif");
    unsigned int wallTex = loadTexture("assets/wall1.jfif");
    unsigned int floorTex = loadTexture("assets/ground.jfif");
    unsigned int posterTex = loadTexture("assets/poster.jfif");

    unsigned int cardTex[5];
    for (int i = 0; i < 5; i++) {
        cardTex[i] = loadTexture("assets/card" + std::to_string(i + 1) + ".jfif");
    }

    // Game & Animation states
    int cameraMode = 1; // 1 = Top-down view, 2 = Player view
    bool cardPlayed[5] = { false, false, false, false, false };
    float currentZ[5] = { 1.1f, 1.1f, 1.1f, 1.1f, 1.1f };
    float currentAngle[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    float currentY[5] = { 0.03f, 0.03f, 0.03f, 0.03f, 0.03f };

    bool gKeyPressed = false;
    bool rKeyPressed = false;
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();

        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        // Requirement 2: Keys 1 and 2 switch camera states
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
            cameraMode = 1;
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
            cameraMode = 2;
        // Requirement 4: Press G to play a random unplayed card
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && !gKeyPressed) {
            gKeyPressed = true;
            std::vector<int> available;

            for (int i = 0; i < 5; i++) {
                if (!cardPlayed[i]) available.push_back(i);
            }
            if (!available.empty()) {
                int chosen = available[std::rand() % available.size()];
                cardPlayed[chosen] = true;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE)
            gKeyPressed = false;
        // Press R to reset all cards back to deck
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !rKeyPressed) {
            rKeyPressed = true;
            for (int i = 0; i < 5; i++) cardPlayed[i] = false;
        }
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE)
            rKeyPressed = false;
        glClearColor(0.12f, 0.14f, 0.18f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Camera setup
       // Camera setup
        glm::mat4 view;
        if (cameraMode == 1) {
            view = glm::lookAt(glm::vec3(0.0f, 4.2f, 0.6f),
                glm::vec3(0.0f, 0.0f, 0.2f),
                glm::vec3(0.0f, 0.0f, -1.0f));
        }
        else {
            view = glm::lookAt(glm::vec3(0.0f, 1.85f, 4.2f),
                glm::vec3(0.0f, 0.55f, -0.5f),
                glm::vec3(0.0f, 1.0f, 0.0f));
        }

        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.1f, 100.0f);
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        // 1. Room: Floor & Walls
        glm::mat4 floorMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.5f, 0.0f));
        floorMat = glm::scale(floorMat, glm::vec3(12.0f, 0.1f, 12.0f));
        drawMesh(shaderProgram, cubeVAO, 36, floorMat, glm::vec3(0.8f), true, floorTex);
        glm::mat4 backWall = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.5f, -5.0f));
        backWall = glm::scale(backWall, glm::vec3(12.0f, 6.0f, 0.2f));
        drawMesh(shaderProgram, cubeVAO, 36, backWall, glm::vec3(0.85f), true, wallTex);
        glm::mat4 leftWall = glm::translate(glm::mat4(1.0f), glm::vec3(-6.0f, 1.5f, 0.0f));
        leftWall = glm::scale(leftWall, glm::vec3(0.2f, 6.0f, 12.0f));
        drawMesh(shaderProgram, cubeVAO, 36, leftWall, glm::vec3(0.8f), true, wallTex);
        glm::mat4 rightWall = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 1.5f, 0.0f));
        rightWall = glm::scale(rightWall, glm::vec3(0.2f, 6.0f, 12.0f));
        drawMesh(shaderProgram, cubeVAO, 36, rightWall, glm::vec3(0.8f), true, wallTex);
        // 2. Torn Poster on Back Wall (Requirement 6)
        glm::mat4 posterMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.8f, -4.88f));
        posterMat = glm::scale(posterMat, glm::vec3(2.2f, 1.6f, 1.0f));
        drawMesh(shaderProgram, posterVAO, 9, posterMat, glm::vec3(1.0f), true, posterTex);
        // 3. Table Top & 4 Legs
        glm::mat4 tableTop = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.05f, 0.0f));
        tableTop = glm::scale(tableTop, glm::vec3(4.2f, 0.1f, 3.0f));
        drawMesh(shaderProgram, cubeVAO, 36, tableTop, glm::vec3(1.0f), true, tableTex);
        float legX[2] = { -1.8f, 1.8f };
        float legZ[2] = { -1.2f, 1.2f };
        for (int lx = 0; lx < 2; lx++) {
            for (int lz = 0; lz < 2; lz++) {
                glm::mat4 leg = glm::translate(glm::mat4(1.0f), glm::vec3(legX[lx], -0.8f, legZ[lz]));
                leg = glm::scale(leg, glm::vec3(0.15f, 1.4f, 0.15f));
                drawMesh(shaderProgram, cubeVAO, 36, leg, glm::vec3(0.35f, 0.22f, 0.12f), false);
            }
        }
        // Play Zone Border Highlight
        glm::mat4 playZone = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.002f, -0.2f));
        playZone = glm::scale(playZone, glm::vec3(3.4f, 0.005f, 0.9f));
        drawMesh(shaderProgram, cubeVAO, 36, playZone, glm::vec3(0.18f, 0.42f, 0.25f), false);
        // 4. Decor 1: Standing Floor Lamp (Back Left)
        glm::mat4 lampBase = glm::translate(glm::mat4(1.0f), glm::vec3(-4.2f, -1.4f, -3.8f));
        lampBase = glm::scale(lampBase, glm::vec3(0.6f, 0.1f, 0.6f));
        drawMesh(shaderProgram, cubeVAO, 36, lampBase, glm::vec3(0.2f, 0.2f, 0.22f), false);
        glm::mat4 lampPole = glm::translate(glm::mat4(1.0f), glm::vec3(-4.2f, -0.2f, -3.8f));
        lampPole = glm::scale(lampPole, glm::vec3(0.08f, 2.4f, 0.08f));
        drawMesh(shaderProgram, cubeVAO, 36, lampPole, glm::vec3(0.75f, 0.65f, 0.3f), false);
        glm::mat4 lampShade = glm::translate(glm::mat4(1.0f), glm::vec3(-4.2f, 1.1f, -3.8f));
        lampShade = glm::scale(lampShade, glm::vec3(0.65f, 0.55f, 0.65f));
        drawMesh(shaderProgram, cubeVAO, 36, lampShade, glm::vec3(0.95f, 0.85f, 0.55f), false);
        // 5. Decor 2: Potted Cactus (Back Right)
        glm::mat4 pot = glm::translate(glm::mat4(1.0f), glm::vec3(4.2f, -1.2f, -3.8f));
        pot = glm::scale(pot, glm::vec3(0.55f, 0.5f, 0.55f));
        drawMesh(shaderProgram, cubeVAO, 36, pot, glm::vec3(0.72f, 0.35f, 0.2f), false);
        glm::mat4 cactusTrunk = glm::translate(glm::mat4(1.0f), glm::vec3(4.2f, -0.45f, -3.8f));
        cactusTrunk = glm::scale(cactusTrunk, glm::vec3(0.22f, 1.1f, 0.22f));
        drawMesh(shaderProgram, cubeVAO, 36, cactusTrunk, glm::vec3(0.18f, 0.58f, 0.24f), false);
        glm::mat4 cactusArmL = glm::translate(glm::mat4(1.0f), glm::vec3(3.98f, -0.35f, -3.8f));
        cactusArmL = glm::scale(cactusArmL, glm::vec3(0.25f, 0.14f, 0.14f));
        drawMesh(shaderProgram, cubeVAO, 36, cactusArmL, glm::vec3(0.18f, 0.58f, 0.24f), false);
        glm::mat4 cactusArmLUp = glm::translate(glm::mat4(1.0f), glm::vec3(3.9f, -0.15f, -3.8f));
        cactusArmLUp = glm::scale(cactusArmLUp, glm::vec3(0.14f, 0.35f, 0.14f));
        drawMesh(shaderProgram, cubeVAO, 36, cactusArmLUp, glm::vec3(0.18f, 0.58f, 0.24f), false);
        // 6. Decor 3: Retro Arcade Machine (Side Wall)
        glm::mat4 arcadeBody = glm::translate(glm::mat4(1.0f), glm::vec3(-4.8f, -0.4f, 0.5f));
        arcadeBody = glm::scale(arcadeBody, glm::vec3(0.9f, 2.1f, 0.9f));
        drawMesh(shaderProgram, cubeVAO, 36, arcadeBody, glm::vec3(0.15f, 0.22f, 0.55f), false);
        glm::mat4 arcadeScreen = glm::translate(glm::mat4(1.0f), glm::vec3(-4.33f, 0.15f, 0.5f));
        arcadeScreen = glm::scale(arcadeScreen, glm::vec3(0.05f, 0.55f, 0.65f));
        drawMesh(shaderProgram, cubeVAO, 36, arcadeScreen, glm::vec3(0.2f, 0.85f, 0.9f), false);
        // 7. Animated Cards (Requirements 2, 3, 4 & Note 2)
        for (int i = 0; i < 5; i++) {
            float xPos = -1.3f + i * 0.65f;
            // Target Z: -0.2f in Play Zone if played, 1.05f in Deck if unplayed
            float targetZ = cardPlayed[i] ? -0.2f : 1.05f;
            // Target Angle & Height: Only unplayed deck cards stand up in Camera Mode 2
            bool shouldStand = (cameraMode == 2 && !cardPlayed[i]);
            float targetAngle = shouldStand ? 75.0f : 0.0f;

            float targetY = shouldStand ? 0.35f : 0.015f;
            // Smoothly interpolate toward targets using deltaTime
            currentZ[i] = lerp(currentZ[i], targetZ, 7.0f, deltaTime);
            currentAngle[i] = lerp(currentAngle[i], targetAngle, 8.0f, deltaTime);
            currentY[i] = lerp(currentY[i], targetY, 8.0f, deltaTime);
            glm::mat4 cardMat = glm::translate(glm::mat4(1.0f), glm::vec3(xPos, currentY[i], currentZ[i]));
            cardMat = glm::rotate(cardMat, glm::radians(currentAngle[i]), glm::vec3(1.0f, 0.0f, 0.0f));
            cardMat = glm::scale(cardMat, glm::vec3(0.48f, 0.015f, 0.72f));
            drawMesh(shaderProgram, cubeVAO, 36, cardMat, glm::vec3(1.0f), true, cardTex[i]);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
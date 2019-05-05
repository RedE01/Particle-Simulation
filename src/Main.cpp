#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#define PARTICLES 1000

std::string readStringFromFile(std::string filePath) {
    std::ifstream fileStream;
    fileStream.open(filePath.c_str());
    std::stringstream fileBuffer;
    fileBuffer << fileStream.rdbuf();
    fileStream.close();
    return fileBuffer.str();
}

unsigned int createShader(std::string shaderSource, GLenum shaderType) {
    const char* temp = shaderSource.c_str();
    unsigned int shaderId = glCreateShader(shaderType);
    glShaderSource(shaderId, 1, &temp, NULL);
    glCompileShader(shaderId);

    int compileStatus;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);
    if(compileStatus != GL_TRUE) {
        int infoLogLen = 0;
        char infoLog[256];
        glGetShaderInfoLog(shaderId, 256, &infoLogLen, infoLog);
        std::cout << "Error in " << (shaderType == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << " shader" << std::endl;
        std::cout << infoLog << std::endl;
    }
    return shaderId;
}

unsigned int createShaderProgram(unsigned int vertexShaderId, unsigned int fragmentShaderId) {
    unsigned int programId = glCreateProgram();

    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);
    glLinkProgram(programId);

    int programLinkStatus;
    glGetProgramiv(programId, GL_LINK_STATUS, &programLinkStatus);
    if(programLinkStatus != GL_TRUE) {
        int infoLogLen = 0;
        char infoLog[256];
        glGetProgramInfoLog(programId, 256, &infoLogLen, infoLog);
        std::cout << infoLog << std::endl;
    }

    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);

    return programId;
}

int main(int argv, char* argc[]) {
    //Remove executable name from filepath
    std::string executablePath = argc[0];
    {
        auto it = executablePath.end();
        for(; it != executablePath.begin() && *it != '/'; --it);
        executablePath.erase(++it, executablePath.end());
    }

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    glm::vec2 screenSize(1280, 720);
    window = glfwCreateWindow(screenSize.x, screenSize.y, "Particle Simulation", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    if(glewInit() != GLEW_OK) {
        std::cout << "FAILED TO INIT GLEW" << std::endl;
        return -1;
    }
    // glfwSwapInterval(0);

    unsigned int vertShaderId = createShader(readStringFromFile(executablePath + "vertexShader.glsl"), GL_VERTEX_SHADER);
    unsigned int fragShaderId = createShader(readStringFromFile(executablePath + "fragmentShader.glsl"), GL_FRAGMENT_SHADER);
    unsigned int shaderProgramId = createShaderProgram(vertShaderId, fragShaderId);
    glUseProgram(shaderProgramId);

    glm::vec2 offsets[PARTICLES];
    for(int i = 0; i < PARTICLES; ++i) {
        offsets[i] = glm::vec2(float(std::rand()) / float(INT_MAX) * 2.0f - 1.0f, float(std::rand() / float(INT_MAX) * 2.0f - 1.0f));
    }

    int offsetsUniformLocation = glGetUniformLocation(shaderProgramId, "offsets");
    glUniform2fv(offsetsUniformLocation, PARTICLES, &(offsets[0][0]));

    int screenSizeUniformLocation = glGetUniformLocation(shaderProgramId, "screenSize");
    glUniform2f(screenSizeUniformLocation, screenSize.x, screenSize.y);

    float vertexData[] {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f,  1.0f
    };

    unsigned int indexData[] {
        0, 1, 2,
        2, 3, 0
    };

    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    
    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now(), end;
    double timer = 0.0f;
    int fps = 0;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        end = std::chrono::high_resolution_clock::now();
        std::chrono::high_resolution_clock::duration dur = end - start;
        start = std::chrono::high_resolution_clock::now();
        timer += dur.count() / 1000000000.0f;
        ++fps;
        if(timer >= 1.000) {
            std::cout << fps << std::endl;
            fps = 0;
            timer -= 1.000;
        }

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElementsInstanced(GL_TRIANGLES, sizeof(indexData) / sizeof(unsigned int) , GL_UNSIGNED_INT, 0, PARTICLES);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
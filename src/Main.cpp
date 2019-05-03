#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

std::string readStringFromFile(const char* filePath) {
    std::ifstream fileStream;
    fileStream.open(filePath);
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

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */

    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Particle Simulation", NULL, NULL);
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

    unsigned int vertShaderId = createShader(readStringFromFile("src/vertexShader.glsl"), GL_VERTEX_SHADER);
    unsigned int fragShaderId = createShader(readStringFromFile("src/fragmentShader.glsl"), GL_FRAGMENT_SHADER);
    unsigned int shaderProgramId = createShaderProgram(vertShaderId, fragShaderId);
    glUseProgram(shaderProgramId);

    float vertexData[] {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f
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

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, sizeof(indexData) / sizeof(unsigned int) , GL_UNSIGNED_INT, 0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
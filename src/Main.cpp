#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Particle.h"

#define PARTICLES 10000

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
        throw std::runtime_error("Shader error");
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
        throw std::runtime_error("Program error");
    }

    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);

    return programId;
}

bool firstCursorPosCallback = true;
glm::vec2 prevCursorPos, deltaCursorPos = glm::vec2(0.0f);
static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
	if (firstCursorPosCallback) {
		firstCursorPosCallback = false;
	}
	else {
		deltaCursorPos = glm::vec2(xpos - prevCursorPos.x, ypos - prevCursorPos.y);
	}
	prevCursorPos = glm::vec2(xpos, ypos);
}

int main(int argv, char* argc[]) {
    //Remove executable name from filepath
    std::string executablePath = argc[0];
    {
		int i = executablePath.length() - 1;
		for (; i > -1 && executablePath[i] != '/' && executablePath[i] != '\\'; --i);
        executablePath.erase(i + 1, executablePath.length() - 1);
    }

	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);

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

	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    if(glewInit() != GLEW_OK) {
        std::cout << "FAILED TO INIT GLEW" << std::endl;
        return -1;
    }
    glfwSwapInterval(0);

    unsigned int vertShaderId = createShader(readStringFromFile(executablePath + "vertexShader.glsl"), GL_VERTEX_SHADER);
    unsigned int fragShaderId = createShader(readStringFromFile(executablePath + "fragmentShader.glsl"), GL_FRAGMENT_SHADER);
    unsigned int shaderProgramId = createShaderProgram(vertShaderId, fragShaderId);
    glUseProgram(shaderProgramId);

    Particle* particles = new Particle[PARTICLES];
    glm::vec3* offsets = new glm::vec3[PARTICLES];
    for(int i = 0; i < PARTICLES; ++i) {
        particles[i].pos = glm::vec3(float(std::rand()) / float(RAND_MAX) * 2.0f - 1.0f, float(std::rand() / float(RAND_MAX) * 2.0f - 1.0f), float(std::rand()) / float(RAND_MAX) * 2.0f - 1.0f);
		particles[i].pos *= 10;
    }

	glm::mat4 perspectiveMatrix = glm::perspective(90.0f, screenSize.x / screenSize.y, 0.1f, 100.0f);
	int perspectiveMatUniformLocation = glGetUniformLocation(shaderProgramId, "projMat");
	glUniformMatrix4fv(perspectiveMatUniformLocation, 1, GL_FALSE, &(perspectiveMatrix[0][0]));

	glm::mat4 viewMat = glm::translate(glm::mat4(1.0f), -cameraPos);
	int viewMatUniformLocation = glGetUniformLocation(shaderProgramId, "viewMat");
	glUniformMatrix4fv(viewMatUniformLocation, 1, GL_FALSE, &(viewMat[0][0]));

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
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
    
    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);

    unsigned int instancedVbo;
    glGenBuffers(1, &instancedVbo);
    glBindBuffer(GL_ARRAY_BUFFER, instancedVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * PARTICLES, &particles[0], GL_DYNAMIC_DRAW);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)0);
    glVertexAttribDivisor(1, 1);

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now(), end;
    double timer = 0.0f;
    int fps = 0;

    double deltaTime = 1.00;
	float cameraRotation = 0.0f;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
		cameraRotation += deltaCursorPos.x * 0.001f;
		glm::vec3 forwardVec = glm::rotate(glm::mat4(1.0f), -cameraRotation, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
		glm::vec3 rightVec = glm::cross(forwardVec, glm::vec3(0.0f, 1.0f, 0.0f));

		if (glfwGetKey(window, GLFW_KEY_W)) cameraPos += forwardVec * float(deltaTime * 5.0f);
		if (glfwGetKey(window, GLFW_KEY_S)) cameraPos -= forwardVec * float(deltaTime * 5.0f);
		if (glfwGetKey(window, GLFW_KEY_A)) cameraPos -= rightVec * float(deltaTime * 5.0f);
		if (glfwGetKey(window, GLFW_KEY_D)) cameraPos += rightVec * float(deltaTime * 5.0f);

		viewMat = glm::rotate(glm::mat4(1.0f), cameraRotation, glm::vec3(0.0f, 1.0f, 0.0f));
		viewMat = glm::translate(viewMat, -cameraPos);
		glUniformMatrix4fv(viewMatUniformLocation, 1, GL_FALSE, &(viewMat[0][0]));

        for(int i = 0; i < PARTICLES; ++i) {
			particles[i].applyForce(glm::vec3(0.0f), 2.0f * deltaTime);
        }
        glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * PARTICLES, &(particles[0]), GL_DYNAMIC_DRAW);

		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, true);

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElementsInstanced(GL_TRIANGLES, sizeof(indexData) / sizeof(unsigned int) , GL_UNSIGNED_INT, 0, PARTICLES);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        end = std::chrono::high_resolution_clock::now();
        std::chrono::high_resolution_clock::duration dur = end - start;
        start = std::chrono::high_resolution_clock::now();
        deltaTime = dur.count() / 1000000000.0f;
        timer += deltaTime;
        ++fps;
        if(timer >= 1.000) {
            std::cout << fps << std::endl;
            fps = 0;
            timer -= 1.000;
        }

		deltaCursorPos = glm::vec2(0.0f);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
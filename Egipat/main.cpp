/**
 * @file main.cpp
 * @author Jovan Ivosevic
 * @brief Base project for Computer Graphics course
 * @version 0.1
 * @date 2022-10-09
 *
 * @copyright Copyright (c) 2022
 *
 */
// ReSharper disable All
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <thread>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "camera.hpp"
#include "irenderable.hpp"
#include "shader.hpp"
#include "model.hpp"

const int WindowWidth = 800;
const int WindowHeight = 800;
const std::string WindowTitle = "Egipat";
const float TargetFPS = 60.0f;
const float TargetFrameTime = 1.0f / TargetFPS;
OrbitalCamera Camera(90.0f, 5.0f, 3.0f, 4.0f);

float FrameStartTime = glfwGetTime();
float FrameEndTime = glfwGetTime();
float dt = FrameEndTime - FrameStartTime;

static void
processInput(GLFWwindow* window, float &x, float &y, float &z, float dt) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        Camera.Move(0.0f, 1.0f, dt);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        Camera.Move(-1.0f, 0.0f, dt);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        Camera.Move(0.0f, -1.0f, dt);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        Camera.Move(1.0f, 0.0f, dt);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		y += 0.1;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		x -= 0.1;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		y -= 0.1;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		x += 0.1;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		z += 0.1;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		z -= 0.1;
}

float LastX = WindowWidth / 2;
float LastY = WindowHeight / 2;
bool FirstMouse = true;

void
mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (FirstMouse) {
        LastX = xpos;
        LastY = ypos;
        FirstMouse = false;
    }

	float xoffset = xpos - LastX;
	float yoffset = LastY - ypos;
	LastX = xpos;
	LastY = ypos;

    Camera.Rotate(xoffset, yoffset, dt);
}  

/**
 * @brief Error callback function for GLFW. See GLFW docs for details
 *
 * @param error Error code
 * @param description Error message
 */
static void
ErrorCallback(int error, const char* description) {
    std::cerr << "GLFW Error: " << description << std::endl;
}

int main() {
    GLFWwindow* Window = 0;
    if (!glfwInit()) {
        std::cerr << "Failed to init glfw" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwSetErrorCallback(ErrorCallback);

    Window = glfwCreateWindow(WindowWidth, WindowHeight, WindowTitle.c_str(), 0, 0);
    if (!Window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(Window);
    
    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(Window, mouse_callback);

    GLenum GlewError = glewInit();
    if (GlewError != GLEW_OK) {
        std::cerr << "Failed to init glew: " << glewGetErrorString(GlewError) << std::endl;
        glfwTerminate();
        return -1;
    }

    Shader Basic("shaders/basic.vert", "shaders/basic.frag");
    
    Camera.mPosition = glm::vec3(0.0f, 0.17f,  9.0f);
    Camera.mFront = glm::vec3(0.0f, 0.0f, -1.0f);
    float RenderDistance = 100.0f;
    float NearDistance = 0.1f;
    float AspectRatio = WindowWidth / (float)WindowHeight;
    glViewport(0, 0, WindowWidth, WindowHeight);

    Model Moon("res/moon/moon.obj");
    if(!Moon.Load()) {
        std::cerr << "Failed to load model" << std::endl;
        glfwTerminate();
        return -1;
    }

    Model Cat("res/cat/cat.obj");
    if(!Cat.Load()) {
        std::cerr << "Failed to load model" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glEnable(GL_DEPTH_TEST);
    glEnable (GL_CULL_FACE);

    std::vector<float> SandVertices = {
        -1.0f, 0.0f,  1.0f, 0.17f, 0.17f, 0.19f,
         1.0f, 0.0f,  1.0f, 0.17f, 0.17f, 0.19f,
         1.0f, 0.0f, -1.0f, 0.17f, 0.17f, 0.19f,
    	 1.0f, 0.0f, -1.0f, 0.17f, 0.17f, 0.19f,
    	-1.0f, 0.0f, -1.0f, 0.17f, 0.17f, 0.19f,
    	-1.0f, 0.0f,  1.0f, 0.17f, 0.17f, 0.19f
    };

    std::vector<float> PyramidVertices = {
        // X    Y      Z       R    G     B
        -1.0f, 0.0f, -1.0f, 0.07f, 0.07f, 0.14f,
        -1.0f, 0.0f,  1.0f, 0.07f, 0.07f, 0.14f,
         1.0f, 0.0f, -1.0f, 0.07f, 0.07f, 0.14f,
         1.0f, 0.0f, -1.0f, 0.07f, 0.07f, 0.14f,
        -1.0f, 0.0f,  1.0f, 0.07f, 0.07f, 0.14f,
         1.0f, 0.0f,  1.0f, 0.07f, 0.07f, 0.14f,

         0.0f, 1.5f,  0.0f, 0.36f, 0.33f, 0.42f,
    	-1.0f, 0.0f,  1.0f, 0.36f, 0.33f, 0.42f,
         1.0f, 0.0f,  1.0f, 0.36f, 0.33f, 0.42f,

         0.0f, 1.5f,  0.0f, 0.36f, 0.36f, 0.42f,
         1.0f, 0.0f,  1.0f, 0.36f, 0.36f, 0.42f,
         1.0f, 0.0f, -1.0f, 0.36f, 0.36f, 0.42f,

         0.0f, 1.5f,  0.0f, 0.36f, 0.33f, 0.42f,
         1.0f, 0.0f, -1.0f, 0.36f, 0.33f, 0.42f,
        -1.0f, 0.0f, -1.0f, 0.36f, 0.33f, 0.42f,

    	 0.0f, 1.5f,  0.0f, 0.36f, 0.36f, 0.42f,
        -1.0,  0.0f, -1.0f, 0.36f, 0.36f, 0.42f,
        -1.0,  0.0f,  1.0f, 0.36f, 0.36f, 0.42f
    };

    std::vector<float> RugVertices = {
        -0.5f,  0.48f, -0.5f, 0.2f, 0.16f, 0.15f,
         0.5f,  0.48f, -0.5f, 0.2f, 0.16f, 0.15f,
         0.5f,  0.50f, -0.5f, 0.2f, 0.16f, 0.15f,
         0.5f,  0.50f, -0.5f, 0.2f, 0.16f, 0.15f,
        -0.5f,  0.50f, -0.5f, 0.2f, 0.16f, 0.15f,
        -0.5f,  0.48f, -0.5f, 0.2f, 0.16f, 0.15f,

        -0.5f,  0.48f,  0.5f, 0.2f, 1.16f, 0.15f,
         0.5f,  0.48f,  0.5f, 0.2f, 1.16f, 0.15f,
         0.5f,  0.50f,  0.5f, 0.2f, 1.16f, 0.15f,
         0.5f,  0.50f,  0.5f, 0.2f, 1.16f, 0.15f,
        -0.5f,  0.50f,  0.5f, 0.2f, 1.16f, 0.15f,
        -0.5f,  0.48f,  0.5f, 0.2f, 1.16f, 0.15f,

        -0.5f,  0.50f,  0.5f, 0.2f, 0.16f, 1.15f,
        -0.5f,  0.50f, -0.5f, 0.2f, 0.16f, 1.15f,
        -0.5f,  0.48f, -0.5f, 0.2f, 0.16f, 1.15f,
        -0.5f,  0.48f, -0.5f, 0.2f, 0.16f, 1.15f,
        -0.5f,  0.48f,  0.5f, 0.2f, 0.16f, 1.15f,
        -0.5f,  0.50f,  0.5f, 0.2f, 0.16f, 1.15f,

         0.5f,  0.50f,  0.5f, 0.2f, 0.16f, 0.15f,
         0.5f,  0.48f, -0.5f, 0.2f, 0.16f, 0.15f,
         0.5f,  0.50f, -0.5f, 0.2f, 0.16f, 0.15f,
         0.5f,  0.48f, -0.5f, 0.2f, 0.16f, 0.15f,
         0.5f,  0.50f,  0.5f, 0.2f, 0.16f, 0.15f,
         0.5f,  0.48f,  0.5f, 0.2f, 0.16f, 0.15f,

        -0.5f,  0.48f, -0.5f, 0.13f, 0.30f, 0.32f,
         0.5f,  0.48f, -0.5f, 0.13f, 0.30f, 0.32f,
         0.5f,  0.48f,  0.5f, 0.13f, 0.30f, 0.32f,
         0.5f,  0.48f,  0.5f, 0.13f, 0.30f, 0.32f,
        -0.5f,  0.48f,  0.5f, 0.13f, 0.30f, 0.32f,
        -0.5f,  0.48f, -0.5f, 0.13f, 0.30f, 0.32f,

        -0.5f,  0.50f, -0.5f, 0.22f, 0.21f, 0.15f,
         0.5f,  0.50f,  0.5f, 0.22f, 0.21f, 0.15f,
         0.5f,  0.50f, -0.5f, 0.22f, 0.21f, 0.15f,
        -0.5f,  0.50f,  0.5f, 0.22f, 0.21f, 0.15f,
         0.5f,  0.50f,  0.5f, 0.22f, 0.21f, 0.15f,
        -0.5f,  0.50f, -0.5f, 0.22f, 0.21f, 0.15f
    };

    unsigned SandVAO;
    glGenVertexArrays(1, &SandVAO);
    glBindVertexArray(SandVAO);
    unsigned SandVBO;
    glGenBuffers(1, &SandVBO);
    glBindBuffer(GL_ARRAY_BUFFER, SandVBO);
    glBufferData(GL_ARRAY_BUFFER, SandVertices.size() * sizeof(float), SandVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    unsigned PyramidOfKhafreVAO;
    glGenVertexArrays(1, &PyramidOfKhafreVAO);
    glBindVertexArray(PyramidOfKhafreVAO);
    unsigned PyramidOfKhafreVBO;
    glGenBuffers(1, &PyramidOfKhafreVBO);
    glBindBuffer(GL_ARRAY_BUFFER, PyramidOfKhafreVBO);
    glBufferData(GL_ARRAY_BUFFER, PyramidVertices.size() * sizeof(float), PyramidVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned PyramidOfMenkaureVAO;
    glGenVertexArrays(1, &PyramidOfMenkaureVAO);
    glBindVertexArray(PyramidOfMenkaureVAO);
    unsigned PyramidOfMenkaureVBO;
    glGenBuffers(1, &PyramidOfMenkaureVBO);
    glBindBuffer(GL_ARRAY_BUFFER, PyramidOfMenkaureVBO);
    glBufferData(GL_ARRAY_BUFFER, PyramidVertices.size() * sizeof(float), PyramidVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned PyramidOfKhufuVAO;
    glGenVertexArrays(1, &PyramidOfKhufuVAO);
    glBindVertexArray(PyramidOfKhufuVAO);
    unsigned PyramidOfKhufuVBO;
    glGenBuffers(1, &PyramidOfKhufuVBO);
    glBindBuffer(GL_ARRAY_BUFFER, PyramidOfKhufuVBO);
    glBufferData(GL_ARRAY_BUFFER, PyramidVertices.size() * sizeof(float), PyramidVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned RugVAO;
    glGenVertexArrays(1, &RugVAO);
    glBindVertexArray(RugVAO);
    unsigned RugVBO;
    glGenBuffers(1, &RugVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RugVBO);
    glBufferData(GL_ARRAY_BUFFER, RugVertices.size() * sizeof(float), RugVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    float x = 0.0f, y = 1.0f, z = 0.0f;
    while (!glfwWindowShouldClose(Window)) {
        glfwPollEvents();
        glClearColor(0.08f, 0.09f, 0.20f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        FrameStartTime = glfwGetTime();
        glUseProgram(Basic.GetId());
        glUniform1i(glGetUniformLocation(Basic.GetId(), "moonflag"), 0);

		glm::mat4 FreeView = glm::lookAt(Camera.mPosition, Camera.mTarget, Camera.mUp);
        Basic.SetView(FreeView);

        glm::mat4 Projection = glm::perspective(45.0f, AspectRatio, NearDistance, RenderDistance);
    	Basic.SetProjection(Projection);
        
        processInput(Window, x, y, z, dt);

        glm::mat4 Model = glm::mat4(1.0f);
        Model = glm::scale(Model, glm::vec3(100.0f));
        Basic.SetModel(Model);
        glBindVertexArray(SandVAO);
        glDrawArrays(GL_TRIANGLES, 0, SandVertices.size() / 6);

        Model = glm::mat4(1.0f);
    	Model = glm::translate(Model, glm::vec3(2.5f, 0.0f, -5.0f));
        Model = glm::scale(Model, glm::vec3(1.46f));
        Model = glm::rotate(Model, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        Basic.SetModel(Model);
        glBindVertexArray(PyramidOfKhufuVAO);
        glDrawArrays(GL_TRIANGLES, 0, PyramidVertices.size() / 6);

        Model = glm::mat4(1.0f);
    	Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));
        Model = glm::scale(Model, glm::vec3(1.47f));
        Model = glm::rotate(Model, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        Basic.SetModel(Model);
        glBindVertexArray(PyramidOfKhafreVAO);
        glDrawArrays(GL_TRIANGLES, 0, PyramidVertices.size() / 6);

        Model = glm::mat4(1.0f);
    	Model = glm::translate(Model, glm::vec3(-1.0f, 0.0f, 3.0f));
        Model = glm::scale(Model, glm::vec3(0.65f));
        Model = glm::rotate(Model, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        Basic.SetModel(Model);
        glBindVertexArray(PyramidOfMenkaureVAO);
        glDrawArrays(GL_TRIANGLES, 0, PyramidVertices.size() / 6);


        Model = glm::mat4(1.0f);
    	Model = glm::translate(Model, glm::vec3(0.0f + x, 0.3 * cos(glfwGetTime()) + y, 1.5f + z));
        Model = glm::rotate(Model, glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        Basic.SetModel(Model);
        glBindVertexArray(RugVAO);
        glDrawArrays(GL_TRIANGLES, 0, RugVertices.size() / 6);

        Model = glm::mat4(1.0f);
        Model = glm::translate(Model, glm::vec3(-0.3f, 0.0f, 3.7f));
        Model = glm::rotate(Model, glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        Basic.SetModel(Model);
        Cat.Render();

        glUniform1i(glGetUniformLocation(Basic.GetId(), "moonflag"), 1);
        Model = glm::mat4(1.0f);
        Model = glm::translate(Model, glm::vec3(-7.0f, 7.0f, -9.0f));
        Basic.SetModel(Model);
        Moon.Render();

        glUseProgram(0);
        glfwSwapBuffers(Window);

        FrameEndTime = glfwGetTime();
        dt = FrameEndTime - FrameStartTime;
        if (dt < TargetFPS) {
            int DeltaMS = (int)((TargetFrameTime - dt) * 1e3f);
            std::this_thread::sleep_for(std::chrono::milliseconds(DeltaMS));
            FrameEndTime = glfwGetTime();
        }
        dt = FrameEndTime - FrameStartTime;
    }

    glfwTerminate();
    return 0;
}

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
#include "texture.hpp"

const int WindowWidth = 800;
const int WindowHeight = 800;
const std::string WindowTitle = "Egipat";
const float TargetFPS = 60.0f;
const float TargetFrameTime = 1.0f / TargetFPS;
const unsigned int Stride = 5 * sizeof(float);
OrbitalCamera Camera(90.0f, 5.0f, 3.0f, 4.0f);

float FrameStartTime = (float)glfwGetTime();
float FrameEndTime = (float)glfwGetTime();
float dt = FrameEndTime - FrameStartTime;

static void
processInput(GLFWwindow* window, float &x, float &y, float &z, float dt) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        Camera.mMoveFaster = true;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        Camera.mMoveFaster = false;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        Camera.Move(0.0f, 1.0f, dt);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        Camera.Move(-1.0f, 0.0f, dt);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        Camera.Move(0.0f, -1.0f, dt);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        Camera.Move(1.0f, 0.0f, dt);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		y += 0.1f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		x -= 0.1f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		y -= 0.1f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		x += 0.1f;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		z += 0.1f;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		z -= 0.1f;
}

float LastX = WindowWidth / 2;
float LastY = WindowHeight / 2;
bool FirstMouse = true;

void
mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (FirstMouse) {
        LastX = (float)xpos;
        LastY = (float)ypos;
        FirstMouse = false;
    }

	float xoffset = (float)xpos - LastX;
	float yoffset = LastY - (float)ypos;
	LastX = (float)xpos;
	LastY = (float)ypos;

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

std::vector<float> calculateTriangleNormals(const std::vector<float>& vertices) {
    std::vector<float> normals;

    for (int i = 0; i < vertices.size(); i += 15) {
        glm::vec3 v1(vertices[i], vertices[i + 1], vertices[i + 2]);
        glm::vec3 v2(vertices[i + 5], vertices[i + 6], vertices[i + 7]);
        glm::vec3 v3(vertices[i + 10], vertices[i + 11], vertices[i + 12]);

        glm::vec3 edge1 = v2 - v1;
        glm::vec3 edge2 = v3 - v1;

        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        for (int i = 0; i < 3; i++) {
		    normals.push_back(normal.x);
		    normals.push_back(normal.y);
		    normals.push_back(normal.z);
        }
    }

    return normals;
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

    Shader AlmightyShader("shaders/basic.vert", "shaders/phong_material_texture.frag");

    Texture textureSand("res/sand/sand.jpg");
    Texture texturePyramid("res/pyramid/pyramid.jpeg");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
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
        -1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
         1.0f, 0.0f,  1.0f, 1.0f, 0.0f,
         1.0f, 0.0f, -1.0f, 1.0f, 1.0f,
    	 1.0f, 0.0f, -1.0f, 1.0f, 1.0f,
    	-1.0f, 0.0f, -1.0f, 0.0f, 1.0f,
    	-1.0f, 0.0f,  1.0f, 0.0f, 0.0f
    };

    std::vector<float> PyramidVertices = {
        -1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f,  1.0f, 1.0f, 0.0f,
         1.0f, 0.0f, -1.0f, 1.0f, 1.0f,
         1.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -1.0f, 0.0f,  1.0f, 0.0f, 1.0f,
         1.0f, 0.0f,  1.0f, 0.0f, 0.0f,

         0.0f, 1.5f,  0.0f, 0.5f, 1.0f,
    	-1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
         1.0f, 0.0f,  1.0f, 1.0f, 0.0f,

         0.0f, 1.5f,  0.0f, 0.5f, 1.0f,
         1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
         1.0f, 0.0f, -1.0f, 1.0f, 0.0f,

         0.0f, 1.5f,  0.0f, 0.5f, 1.0f,
         1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, -1.0f, 1.0f, 0.0f,

    	 0.0f, 1.5f,  0.0f, 0.5f, 1.0f,
        -1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f,  1.0f, 1.0f, 0.0f
    };

    std::vector<float> RugVertices = {
        -0.5f,  0.48f, -0.5f, 0.2f, 0.16f,
         0.5f,  0.48f, -0.5f, 0.2f, 0.16f,
         0.5f,  0.50f, -0.5f, 0.2f, 0.16f,
         0.5f,  0.50f, -0.5f, 0.2f, 0.16f,
        -0.5f,  0.50f, -0.5f, 0.2f, 0.16f,
        -0.5f,  0.48f, -0.5f, 0.2f, 0.16f,

        -0.5f,  0.48f,  0.5f, 0.2f, 1.16f,
         0.5f,  0.48f,  0.5f, 0.2f, 1.16f,
         0.5f,  0.50f,  0.5f, 0.2f, 1.16f,
         0.5f,  0.50f,  0.5f, 0.2f, 1.16f,
        -0.5f,  0.50f,  0.5f, 0.2f, 1.16f,
        -0.5f,  0.48f,  0.5f, 0.2f, 1.16f,

        -0.5f,  0.50f,  0.5f, 0.2f, 0.16f,
        -0.5f,  0.50f, -0.5f, 0.2f, 0.16f,
        -0.5f,  0.48f, -0.5f, 0.2f, 0.16f,
        -0.5f,  0.48f, -0.5f, 0.2f, 0.16f,
        -0.5f,  0.48f,  0.5f, 0.2f, 0.16f,
        -0.5f,  0.50f,  0.5f, 0.2f, 0.16f,

         0.5f,  0.50f,  0.5f, 0.2f, 0.16f,
         0.5f,  0.48f, -0.5f, 0.2f, 0.16f,
         0.5f,  0.50f, -0.5f, 0.2f, 0.16f,
         0.5f,  0.48f, -0.5f, 0.2f, 0.16f,
         0.5f,  0.50f,  0.5f, 0.2f, 0.16f,
         0.5f,  0.48f,  0.5f, 0.2f, 0.16f,

        -0.5f,  0.48f, -0.5f, 0.13f, 0.30f,
         0.5f,  0.48f, -0.5f, 0.13f, 0.30f,
         0.5f,  0.48f,  0.5f, 0.13f, 0.30f,
         0.5f,  0.48f,  0.5f, 0.13f, 0.30f,
        -0.5f,  0.48f,  0.5f, 0.13f, 0.30f,
        -0.5f,  0.48f, -0.5f, 0.13f, 0.30f,

        -0.5f,  0.50f, -0.5f, 0.22f, 0.21f,
         0.5f,  0.50f,  0.5f, 0.22f, 0.21f,
         0.5f,  0.50f, -0.5f, 0.22f, 0.21f,
        -0.5f,  0.50f,  0.5f, 0.22f, 0.21f,
         0.5f,  0.50f,  0.5f, 0.22f, 0.21f,
        -0.5f,  0.50f, -0.5f, 0.22f, 0.21f
    };

    unsigned SandVAO;
    glGenVertexArrays(1, &SandVAO);
    glBindVertexArray(SandVAO);
    unsigned SandVBO;
    glGenBuffers(1, &SandVBO);
    glBindBuffer(GL_ARRAY_BUFFER, SandVBO);
    glBufferData(GL_ARRAY_BUFFER, SandVertices.size() * sizeof(float), SandVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, Stride, (void*) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, Stride, (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
	std::vector<float> SandNormals = calculateTriangleNormals(SandVertices);
	unsigned SandVBO_normals;
    glGenBuffers(1, &SandVBO_normals);
	glBindBuffer(GL_ARRAY_BUFFER, SandVBO_normals);
	glBufferData(GL_ARRAY_BUFFER, SandNormals.size() * sizeof(float), SandNormals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    unsigned PyramidOfKhafreVAO;
    glGenVertexArrays(1, &PyramidOfKhafreVAO);
    glBindVertexArray(PyramidOfKhafreVAO);
    unsigned PyramidOfKhafreVBO;
    glGenBuffers(1, &PyramidOfKhafreVBO);
    glBindBuffer(GL_ARRAY_BUFFER, PyramidOfKhafreVBO);
    glBufferData(GL_ARRAY_BUFFER, PyramidVertices.size() * sizeof(float), PyramidVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, Stride, (void*) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, Stride, (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    std::vector<float> PyramidOfKhafreNormals = calculateTriangleNormals(SandVertices);
	unsigned PyramidOfKhafreVBO_normals;
    glGenBuffers(1, &PyramidOfKhafreVBO_normals);
	glBindBuffer(GL_ARRAY_BUFFER, PyramidOfKhafreVBO_normals);
	glBufferData(GL_ARRAY_BUFFER, PyramidOfKhafreNormals.size() * sizeof(float), PyramidOfKhafreNormals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned PyramidOfMenkaureVAO;
    glGenVertexArrays(1, &PyramidOfMenkaureVAO);
    glBindVertexArray(PyramidOfMenkaureVAO);
    unsigned PyramidOfMenkaureVBO;
    glGenBuffers(1, &PyramidOfMenkaureVBO);
    glBindBuffer(GL_ARRAY_BUFFER, PyramidOfMenkaureVBO);
    glBufferData(GL_ARRAY_BUFFER, PyramidVertices.size() * sizeof(float), PyramidVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, Stride, (void*) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, Stride, (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    std::vector<float> PyramidOfMenkaureNormals = calculateTriangleNormals(SandVertices);
	unsigned PyramidOfMenkaureVBO_normals;
    glGenBuffers(1, &PyramidOfMenkaureVBO_normals);
	glBindBuffer(GL_ARRAY_BUFFER, PyramidOfMenkaureVBO_normals);
	glBufferData(GL_ARRAY_BUFFER, PyramidOfMenkaureNormals.size() * sizeof(float), PyramidOfMenkaureNormals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned PyramidOfKhufuVAO;
    glGenVertexArrays(1, &PyramidOfKhufuVAO);
    glBindVertexArray(PyramidOfKhufuVAO);
    unsigned PyramidOfKhufuVBO;
    glGenBuffers(1, &PyramidOfKhufuVBO);
    glBindBuffer(GL_ARRAY_BUFFER, PyramidOfKhufuVBO);
    glBufferData(GL_ARRAY_BUFFER, PyramidVertices.size() * sizeof(float), PyramidVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, Stride, (void*) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, Stride, (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    std::vector<float> PyramidOfKhufuNormals = calculateTriangleNormals(SandVertices);
	unsigned PyramidOfKhufuVBO_normals;
    glGenBuffers(1, &PyramidOfKhufuVBO_normals);
	glBindBuffer(GL_ARRAY_BUFFER, PyramidOfKhufuVBO_normals);
	glBufferData(GL_ARRAY_BUFFER, PyramidOfKhufuNormals.size() * sizeof(float), PyramidOfKhufuNormals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned RugVAO;
    glGenVertexArrays(1, &RugVAO);
    glBindVertexArray(RugVAO);
    unsigned RugVBO;
    glGenBuffers(1, &RugVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RugVBO);
    glBufferData(GL_ARRAY_BUFFER, RugVertices.size() * sizeof(float), RugVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, Stride, (void*) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, Stride, (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    std::vector<float> RugNormals = calculateTriangleNormals(SandVertices);
	unsigned RugVBO_normals;
    glGenBuffers(1, &RugVBO_normals);
	glBindBuffer(GL_ARRAY_BUFFER, RugVBO_normals);
	glBufferData(GL_ARRAY_BUFFER, RugNormals.size() * sizeof(float), RugNormals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glUseProgram(AlmightyShader.GetId());
    AlmightyShader.SetUniform3f("uDirLight.Direction", glm::vec3(1.0f, -1.0f, 0.0f));
    AlmightyShader.SetUniform3f("uDirLight.Ka", glm::vec3(1.0f, 1.0f, 1.0f));
    AlmightyShader.SetUniform3f("uDirLight.Kd", glm::vec3(0.0f, 0.0f, 0.1f));
    AlmightyShader.SetUniform3f("uDirLight.Ks", glm::vec3(1.0f));

    AlmightyShader.SetUniform3f("uPointLight.Ka", glm::vec3(0.0f, 0.2f, 0.0f));
    AlmightyShader.SetUniform3f("uPointLight.Kd", glm::vec3(0.0f, 0.5f, 0.0f));
    AlmightyShader.SetUniform3f("uPointLight.Ks", glm::vec3(1.0f));
    AlmightyShader.SetUniform1f("uPointLight.Kc", 1.0f);
    AlmightyShader.SetUniform1f("uPointLight.Kl", 0.092f);
    AlmightyShader.SetUniform1f("uPointLight.Kq", 0.032f);

    AlmightyShader.SetUniform3f("uSpotlight.Position", glm::vec3(0.0f, 3.5f, -2.0f));
    AlmightyShader.SetUniform3f("uSpotlight.Direction", glm::vec3(0.0f, -1.0f, 1.0f));
    AlmightyShader.SetUniform3f("uSpotlight.Ka", glm::vec3(0.2f, 0.0f, 0.0f));
    AlmightyShader.SetUniform3f("uSpotlight.Kd", glm::vec3(0.5f, 0.0f, 0.0f));
    AlmightyShader.SetUniform3f("uSpotlight.Ks", glm::vec3(1.0f));
    AlmightyShader.SetUniform1f("uSpotlight.Kc", 1.0f);
    AlmightyShader.SetUniform1f("uSpotlight.Kl", 0.092f);
    AlmightyShader.SetUniform1f("uSpotlight.Kq", 0.032f);
    AlmightyShader.SetUniform1f("uSpotlight.InnerCutOff", glm::cos(glm::radians(12.5f)));
    AlmightyShader.SetUniform1f("uSpotlight.OuterCutOff", glm::cos(glm::radians(17.5f)));
    // NOTE(Jovan): Diminishes the light's diffuse component by half, tinting it slightly red
    AlmightyShader.SetUniform1i("uMaterial.Kd", 0);
    // NOTE(Jovan): Makes the object really shiny
    AlmightyShader.SetUniform1i("uMaterial.Ks", 1);
    AlmightyShader.SetUniform1f("uMaterial.Shininess", 128.0f);

    unsigned int vertexLength = Stride / sizeof(float);
    float x = 0.0f, y = 1.0f, z = 0.0f;
    while (!glfwWindowShouldClose(Window)) {
        glfwPollEvents();
        glClearColor(0.08f, 0.09f, 0.20f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        FrameStartTime = (float)glfwGetTime();
        glUseProgram(AlmightyShader.GetId());

		glm::mat4 FreeView = glm::lookAt(Camera.mPosition, Camera.mTarget, Camera.mUp);
        AlmightyShader.SetView(FreeView);

        glm::mat4 Projection = glm::perspective(45.0f, AspectRatio, NearDistance, RenderDistance);
    	AlmightyShader.SetProjection(Projection);
        
        processInput(Window, x, y, z, dt);

        textureSand.Bind();
        glm::mat4 Model = glm::mat4(1.0f);
        Model = glm::scale(Model, glm::vec3(15.0f));
        AlmightyShader.SetModel(Model);
        glBindVertexArray(SandVAO);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)SandVertices.size() / vertexLength);

        texturePyramid.Bind();
        Model = glm::mat4(1.0f);
    	Model = glm::translate(Model, glm::vec3(2.5f, 0.0f, -5.0f));
        Model = glm::scale(Model, glm::vec3(1.46f));
        Model = glm::rotate(Model, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        AlmightyShader.SetModel(Model);
        glBindVertexArray(PyramidOfKhufuVAO);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)PyramidVertices.size() / vertexLength);

        Model = glm::mat4(1.0f);
    	Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));
        Model = glm::scale(Model, glm::vec3(1.47f));
        Model = glm::rotate(Model, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        AlmightyShader.SetModel(Model);
        glBindVertexArray(PyramidOfKhafreVAO);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)PyramidVertices.size() / vertexLength);

        Model = glm::mat4(1.0f);
    	Model = glm::translate(Model, glm::vec3(-1.0f, 0.0f, 3.0f));
        Model = glm::scale(Model, glm::vec3(0.65f));
        Model = glm::rotate(Model, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        AlmightyShader.SetModel(Model);
        glBindVertexArray(PyramidOfMenkaureVAO);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)PyramidVertices.size() / vertexLength);

        Model = glm::mat4(1.0f);
    	Model = glm::translate(Model, glm::vec3(0.0f + x, 0.3 * cos(glfwGetTime()) + y, 1.5f + z));
        Model = glm::rotate(Model, glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        AlmightyShader.SetModel(Model);
        glBindVertexArray(RugVAO);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)RugVertices.size() / vertexLength);

        Model = glm::mat4(1.0f);
        Model = glm::translate(Model, glm::vec3(-0.3f, 0.0f, 3.7f));
        Model = glm::rotate(Model, glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        AlmightyShader.SetModel(Model);
        Cat.Render();
        
        Model = glm::mat4(1.0f);
        Model = glm::translate(Model, glm::vec3(-7.0f, 7.0f, -9.0f));
        AlmightyShader.SetModel(Model);
        Moon.Render();

        glUseProgram(0);
        glfwSwapBuffers(Window);

        FrameEndTime = (float)glfwGetTime();
        dt = FrameEndTime - FrameStartTime;
        if (dt < TargetFPS) {
            int DeltaMS = (int)((TargetFrameTime - dt) * 1e3f);
            std::this_thread::sleep_for(std::chrono::milliseconds(DeltaMS));
            FrameEndTime = (float)glfwGetTime();
        }
        dt = FrameEndTime - FrameStartTime;
    }

    glfwTerminate();
    return 0;
}

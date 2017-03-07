#include "Window.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "Input.h"
#include "Shader.h"
#include "Texture.h"
#include "Controls.h"


static const int TRIANGLE_VERTICES = 3;
static const int CUBE_VERTICES = 12 * TRIANGLE_VERTICES;

// Cube data
static const GLfloat g_cube_vertex_buffer_data[] = {
        -1.0f,-1.0f,-1.0f, // triangle 1 : begin
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, // triangle 1 : end
        1.0f, 1.0f,-1.0f,  // triangle 2 : begin
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f, // triangle 2 : end
        1.0f,-1.0f, 1.0f,  // etc...
        -1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f
};

// Color data for each vertex
static GLfloat g_cube_color_buffer_data[CUBE_VERTICES * 3] = {
        0.583f,  0.771f,  0.014f,
        0.609f,  0.115f,  0.436f,
        0.327f,  0.483f,  0.844f,
        0.822f,  0.569f,  0.201f,
        0.435f,  0.602f,  0.223f,
        0.310f,  0.747f,  0.185f,
        0.597f,  0.770f,  0.761f,
        0.559f,  0.436f,  0.730f,
        0.359f,  0.583f,  0.152f,
        0.483f,  0.596f,  0.789f,
        0.559f,  0.861f,  0.639f,
        0.195f,  0.548f,  0.859f,
        0.014f,  0.184f,  0.576f,
        0.771f,  0.328f,  0.970f,
        0.406f,  0.615f,  0.116f,
        0.676f,  0.977f,  0.133f,
        0.971f,  0.572f,  0.833f,
        0.140f,  0.616f,  0.489f,
        0.997f,  0.513f,  0.064f,
        0.945f,  0.719f,  0.592f,
        0.543f,  0.021f,  0.978f,
        0.279f,  0.317f,  0.505f,
        0.167f,  0.620f,  0.077f,
        0.347f,  0.857f,  0.137f,
        0.055f,  0.953f,  0.042f,
        0.714f,  0.505f,  0.345f,
        0.783f,  0.290f,  0.734f,
        0.722f,  0.645f,  0.174f,
        0.302f,  0.455f,  0.848f,
        0.225f,  0.587f,  0.040f,
        0.517f,  0.713f,  0.338f,
        0.053f,  0.959f,  0.120f,
        0.393f,  0.621f,  0.362f,
        0.673f,  0.211f,  0.457f,
        0.820f,  0.883f,  0.371f,
        0.982f,  0.099f,  0.879f
};


Window::Window(int width, int height, const std::string name)
{
    if (!glfwInit())
    {
        std::cerr << "Error initializing in GLFWn" << std::endl;
        return;
    }

    // Use OpenGL 4.1 without forward-compatibility.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);  // Use FSAA x4.

    window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Can't create the GLFW window. " << std::endl;
        glfwTerminate();
        return;
    }
}


// Initialize OpenGL context.
void Window::Initialize()
{
    glfwMakeContextCurrent(window);

    // Initialize GLEW.
    glewExperimental = GL_TRUE;  // Required for core profile in OpenGL.
    GLenum result = glewInit();
    if (result != GLEW_OK)
    {
        std::cerr << "Error initializing in GLEWn" << std::endl;
        std::cerr << glewGetErrorString(result) << std::endl;
        return;
    }
}


void Window::Run()
{
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Hide the mouse and enable unlimited movement.
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Clear the screen.
    glClearColor(0.f, 0.f, 0.f, 0.f);

    // Create Vertex Array Object (VAO).
    GLuint vertexArrayID;
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders(
        "../lesson 18 – billboards/VertexShader.glsl",
        "../lesson 18 – billboards/FragmentShader.glsl"
    );

    // Vertex shader
    GLint CameraRight_worldspace_ID  = glGetUniformLocation(programID, "CameraRight_worldspace");
    GLint CameraUp_worldspace_ID  = glGetUniformLocation(programID, "CameraUp_worldspace");
    GLint ViewProjMatrixID = glGetUniformLocation(programID, "VP");
    GLint BillboardPosID = glGetUniformLocation(programID, "BillboardPos");
    GLint BillboardSizeID = glGetUniformLocation(programID, "BillboardSize");
    GLint LifeLevelID = glGetUniformLocation(programID, "LifeLevel");
    GLint TextureID  = glGetUniformLocation(programID, "myTextureSampler");
    GLuint texture = loadDDS("../lesson 18 – billboards/ExampleBillboard.DDS");

    // The VBO containing the 4 vertices of the particles.
    static const GLfloat g_vertex_buffer_data[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f,
            0.5f,  0.5f, 0.0f,
    };
    GLuint billboard_vertex_buffer;
    glGenBuffers(1, &billboard_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_DYNAMIC_DRAW);

    // Everything here comes from Tutorial 4
    GLuint cubeProgramID = LoadShaders(
        "../lesson 4 – a colored cube/VertexShader.glsl",
        "../lesson 4 – a colored cube/FragmentShader.glsl"
    );
    GLint cubeMatrixID = glGetUniformLocation(cubeProgramID, "MVP");
    GLuint cubevertexbuffer;
    glGenBuffers(1, &cubevertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, cubevertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_cube_vertex_buffer_data), g_cube_vertex_buffer_data, GL_DYNAMIC_DRAW);

    GLuint cubecolorbuffer;
    glGenBuffers(1, &cubecolorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, cubecolorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_cube_color_buffer_data), g_cube_color_buffer_data, GL_DYNAMIC_DRAW);

    // Enable depth test.
    glEnable(GL_DEPTH_TEST);

    // Accept fragment if it closer to the camera than the former one.
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera.
    glEnable(GL_CULL_FACE);

    double lastTime = glfwGetTime();

    do
    {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        double currentTime = glfwGetTime();
        double delta = currentTime - lastTime;
        lastTime = currentTime;

        computeMatricesFromInputs(window);
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();

        glDisable(GL_BLEND);
        glUseProgram(cubeProgramID);
        glm::mat4 cubeModelMatrix(1.0f);
        cubeModelMatrix = glm::scale(cubeModelMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
        glm::mat4 cubeMVP = ProjectionMatrix * ViewMatrix * cubeModelMatrix;
        glUniformMatrix4fv(cubeMatrixID, 1, GL_FALSE, &cubeMVP[0][0]);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, cubevertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, cubecolorbuffer);
        glVertexAttribPointer(1, 3, GL_FLOAT,  GL_FALSE, 0, (void*)0);

        glDrawArrays(GL_TRIANGLES, 0, 12*3);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        // We will need the camera's position in order to sort the particles
        // w.r.t the camera's distance.
        // There should be a getCameraPosition() function in common/controls.cpp,
        // but this works too.
        glm::vec3 CameraPosition(glm::inverse(ViewMatrix)[3]);

        glm::mat4 ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Use our shader
        glUseProgram(programID);

        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        // Set our "myTextureSampler" sampler to user Texture Unit 0
        glUniform1i(TextureID, 0);

        // This is the only interesting part of the tutorial.
        // This is equivalent to multiplying (1, 0, 0) and (0, 1, 0) by inverse(ViewMatrix).
        // ViewMatrix is orthogonal (it was made this way),
        // so its inverse is also its transpose,
        // and transposing a matrix is "free" (inversing is slooow)
        glUniform3f(CameraRight_worldspace_ID, ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
        glUniform3f(CameraUp_worldspace_ID, ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);

        glUniform3f(BillboardPosID, 0.0f, 0.5f, 0.0f); // The billboard will be just above the cube
        glUniform2f(BillboardSizeID, 1.0f, 0.125f);  // and 1m*12cm, because it matches its 256*32 resolution =)

        // Generate some fake life level and send it to glsl
        float LifeLevel = sin(currentTime) * 0.1f + 0.7f;
        glUniform1f(LifeLevelID, LifeLevel);

        glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // Draw the billboard. This draws a triangle_strip which looks like a quad.
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDisableVertexAttribArray(0);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    while (Input::IsKeyPressed(window, KEYBOARD_KEY::ESC) && glfwWindowShouldClose(window) == 0);

    // Cleanup VBO and shader
    glDeleteBuffers(1, &billboard_vertex_buffer);
    glDeleteProgram(programID);
    glDeleteTextures(1, &texture);
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteProgram(cubeProgramID);
	glDeleteVertexArrays(1, &cubevertexbuffer);
	glDeleteVertexArrays(1, &cubecolorbuffer);

    glfwTerminate();
}

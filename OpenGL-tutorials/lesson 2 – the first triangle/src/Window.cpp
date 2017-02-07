#include "Window.h"
#include <iostream>

#include "Input.h"
#include "Shader.h"

// Triangle data
static const GLfloat g_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f,  1.0f, 0.0f,
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

    // Clear the screen.
    glClearColor(0.f, 0.f, 0.f, 0.f);

    // Create Vertex Array Object (VAO).
    GLuint vertexArrayID;
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);


    GLuint vertexBuffer;
    // Create one buffer and bind it.
    glGenBuffers(1, &vertexBuffer);
    // Set this buffer as active.
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    // Put into bounded array our data.
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    GLuint programID = LoadShaders("../shaders/VertexShader.glsl", "../shaders/FragmentShader.glsl");

    do
    {
        // Clear the screen.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use generated shader.
        glUseProgram(programID);

        // Enable generic vertex attribute array.
        glEnableVertexAttribArray(0);
        // Bind buffer once more time.
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        // Change settings for vertices attributes.
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        // Draw a triangle.
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // Disable generic vertex attribute array.
        glDisableVertexAttribArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    while (Input::IsKeyPressed(window, KEYBOARD_KEY::ESC) && glfwWindowShouldClose(window) == 0);

    glfwTerminate();
}

#include "Window.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Input.h"
#include "Shader.h"

// Triangle data
static const GLfloat g_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f,  1.0f, 0.0f,
};

static const float FOV = glm::radians(45.f);
static const float ASPECT_RATIO = 4.f / 3.f;
static const float Z_NEAR = 0.1f;
static const float Z_FAR = 100.f;


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

    // ----------------------------------------------------------------------------------
    // STEP 1: Load shaders, and prepare views with shaders before rendering a triangle.
    // ----------------------------------------------------------------------------------
    // Load shaders from the GLSL sources.
    GLuint programID = LoadShaders(
            "../lesson 3 – matrices/SimpleTransform.glsl",
            "../lesson 3 – matrices/FragmentShader.glsl"
    );

    // Projection matrix (perspective)
    glm::mat4 projectionView = glm::perspective(FOV, ASPECT_RATIO, Z_NEAR, Z_FAR);
    // Projection matrix (orthogonal)
    // glm::mat4 projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f);

    // Camera view
    glm::mat4 cameraView = glm::lookAt(
            glm::vec3(4, 3, 3),  // Position in world coords.
            glm::vec3(0, 0, 0),  // Looks at the origin
            glm::vec3(0, 1, 0)   // Head is up.
    );
    // Model view
    glm::mat4 modelView = glm::mat4(1.f);
    // Scale source triangle
    // {
    //     2.f, 0.f, 0.f, 0.f,
    //     0.f, 2.f, 0.f, 0.f,
    //     0.f, 0.f, 2.f, 0.f,
    //     0.f, 0.f, 0.f, 1.f
    // };
    // or rotate it (turn on 90 clockwise around X axis)
    // {
    //     1.f, 0.f, 0.f, 0.f,
    //     0.f, 0.f, -1.f, 0.f,
    //     0.f, 1.f, 0.f, 0.f,
    //     0.f, 0.f, 0.f, 1.f
    // };
    // or make a translation
    // {
    //     1.f, 0.f, 0.f, 0.5f,
    //     0.f, 1.f, 0.f, 0.f,
    //     0.f, 0.f, 1.f, 0.f,
    //     0.f, 0.f, 0.f, 1.f
    // };

    // Result matrix
    glm::mat4 mvpView = projectionView * cameraView * modelView;

    // Get a handle for our "MVP" uniform variable.
    GLint matrixID = glGetUniformLocation(programID, "MVP");
    // Transfer data to a shader.
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvpView[0][0]);

    // ----------------------------------------------------------------------------------
    // STEP 2: Bind existing data before rendering.
    // ----------------------------------------------------------------------------------
    GLuint vertexBuffer;
    // Create one buffer and bind it.
    glGenBuffers(1, &vertexBuffer);
    // Set this buffer as active.
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    // Put into bounded array our data.
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    do
    {
        // Clear the screen.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use generated shader.
        glUseProgram(programID);

        // STEP 3: Send our transformation to the currently bound shader, in the "MVP" uniform.
        glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvpView[0][0]);

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

    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &vertexArrayID);

    glfwTerminate();
}

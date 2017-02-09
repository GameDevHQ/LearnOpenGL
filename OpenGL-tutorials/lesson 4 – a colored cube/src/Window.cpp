#include "Window.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Input.h"
#include "Shader.h"

static const int TRIANGLE_VERTICES = 3;
static const int CUBE_VERTICES = 12 * TRIANGLE_VERTICES;

// Triangle data
static const GLfloat g_triangle_vertex_buffer_data[] = {
    -1.0f, -1.0f, 6.0f,
     2.0f, -1.0f, 2.0f,
     0.0f,  1.0f, 2.0f,
};

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
static GLfloat g_color_buffer_data[CUBE_VERTICES * 3] = {
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

    // Load shaders from the GLSL sources.
    GLuint programID = LoadShaders(
            "../lesson 4 – a colored cube/VertexShader.glsl",
            "../lesson 4 – a colored cube/FragmentShader.glsl"
    );

    // Projection matrix (perspective)
    glm::mat4 projectionView = glm::perspective(FOV, ASPECT_RATIO, Z_NEAR, Z_FAR);

    // Camera view
    glm::mat4 cameraView = glm::lookAt(
            glm::vec3(4, 3, -3), // Position in world coords.
            glm::vec3(0, 0, 0),  // Looks at the origin
            glm::vec3(0, 1, 0)   // Head is up.
    );
    // Model view
    glm::mat4 modelView = glm::mat4(1.f);

    // Result matrix
    glm::mat4 mvpView = projectionView * cameraView * modelView;

    // Get a handle for our "MVP" uniform variable.
    GLint matrixID = glGetUniformLocation(programID, "MVP");
    // Transfer data to a shader.
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvpView[0][0]);

    // Bind existing data before rendering.
    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_cube_vertex_buffer_data), g_cube_vertex_buffer_data, GL_STATIC_DRAW);

    // Make the same with the color buffer...
    GLuint colorBuffer;
    glGenBuffers(1, &colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

    // ------------------------------------------------------------
    // EXERCISE 1 [part one]: Draw triangle and cube together
    //
    // Result matrix for triangle;
    // glm::mat4 triangle_mvpView = projectionView * cameraView * modelView;
    //
    // Get a handle for our "MVP" uniform variable.
    // GLint triangle_matrixID = glGetUniformLocation(programID, "MVP");
    //
    // Bind data for triangle.
    // GLuint triangleVertextBuffer;
    // glGenBuffers(1, &triangleVertextBuffer);
    // glBindBuffer(GL_ARRAY_BUFFER, triangleVertextBuffer);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(g_triangle_vertex_buffer_data), g_triangle_vertex_buffer_data, GL_STATIC_DRAW);
    // ------------------------------------------------------------

    // Enable depth test.
    glEnable(GL_DEPTH_TEST);

    // Accept fragment if it closer to the camera than the former one.
    glDepthFunc(GL_LESS);

    // ------------------------------------------------------------
    // EXERCISE 2 & 3 [part one]: Random color for each frame
    // std::srand(50);
    // ------------------------------------------------------------

    do
    {
        // Clear the screen.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader.
        glUseProgram(programID);

        // ------------------------------------------------------------
        // EXERCISE 2 & 3 [part two]: Random color for each frame
        // for (int v = 0; v < CUBE_VERTICES; v++)
        // {
        //    g_color_buffer_data[3 * v + 0] = (std::rand() % 100) / 100.f;
        //    g_color_buffer_data[3 * v + 1] = (std::rand() % 100) / 100.f;
        //    g_color_buffer_data[3 * v + 2] = (std::rand() % 100) / 100.f;
        // }
        //
        // glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
        // ------------------------------------------------------------

        // Send our transformation to the currently bound shader in the "MVP" uniform.
        glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvpView[0][0]);

        // 1st attribute buffer: vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // 2nd attribute buffer: colors
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // Draw the triangle.
        glDrawArrays(GL_TRIANGLES, 0, CUBE_VERTICES);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        // ------------------------------------------------------------
        // EXERCISE 1 [part two]: Draw triangle and cube together
        //
        //glUniformMatrix4fv(matrixID, 1, GL_FALSE, &triangle_mvpView[0][0]);
        //
        // 1st attribute buffer: vertices
        //glEnableVertexAttribArray(0);
        //glBindBuffer(GL_ARRAY_BUFFER, triangleVertextBuffer);
        //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        //
        //2nd attribute buffer: colors
        //glEnableVertexAttribArray(1);
        //glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
        //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        //
        // Draw the triangle.
        //glDrawArrays(GL_TRIANGLES, 0, TRIANGLE_VERTICES);
        //glDisableVertexAttribArray(0);
        //glDisableVertexAttribArray(1);
        // ------------------------------------------------------------

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    while (Input::IsKeyPressed(window, KEYBOARD_KEY::ESC) && glfwWindowShouldClose(window) == 0);

    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &colorBuffer);
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &vertexArrayID);

    glfwTerminate();
}

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../common/shader.h"

const int window_width = 800;
const int window_height = 600;

// Triangle data
static const GLfloat g_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
     0.0f,  1.0f, 0.0f,
};


void ChangeGlfwWindowParams() {
    // Use OpenGL 4.1 with forward-compatibility
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_SAMPLES, 4); // Use FSAA x4
}


void PrepareGlfwWindow(GLFWwindow* window) {
    // Enable keyboard support
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Set to black background
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}


bool CloseWindowIsNotRequired(GLFWwindow *window) {
    bool escape_key_pressed = (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS);
    bool window_should_close = (glfwWindowShouldClose(window) == 0);
    return escape_key_pressed && window_should_close;
}


int main() {
    GLFWwindow * window;
    GLuint VertexArrayID, VertexBuffer, ProgramID;


    if( !glfwInit() ) {
        std::cout << "Error GLFW initialize." << std::endl;
        return -1;
    }

    ChangeGlfwWindowParams();

    window = glfwCreateWindow(window_width, window_height, "Lesson 2 – The first triangle", NULL, NULL);
    if (window == NULL) {
        std::cout << "Can't create GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if ( glewInit() != GLEW_OK ) {
        std::cout << "Error GLEWn initialize." << std::endl;
        return -1;
    }

    PrepareGlfwWindow(window);

    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile GLSL program from the shaders
    ProgramID = LoadShaders("VertexShader.glsl", "FragmentShader.glsl");

    // Create one buffer and bind it with VertexBuffer
    glGenBuffers(1, &VertexBuffer);
    // Use array as a buffer
    glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
    // Put into bounded array our data
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    // Redraw window
    while(CloseWindowIsNotRequired(window)) {

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use generated shader
        glUseProgram(ProgramID);

        // Enable generic vertex attribute array
        glEnableVertexAttribArray(0);
        // Bind buffer once more time
        glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
        // Change settings for vertices attributes
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        // Draw our triangle
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // Disable generic vertex attribute array
        glDisableVertexAttribArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteBuffers(1, &VertexBuffer);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(ProgramID);

    glfwTerminate();

    return 0;
}

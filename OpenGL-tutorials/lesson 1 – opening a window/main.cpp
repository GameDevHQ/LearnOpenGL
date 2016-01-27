#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

const int window_width = 800;
const int window_height = 600;


void ChangeGlfwWindowParams() {
    // Use OpenGL 4.1 without forward-compatibility
    glfwWindowHint(GLFW_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

    glfwWindowHint(GLFW_SAMPLES, 4); // Use FSAA x4
}


void PrepareGlfwWindow(GLFWwindow* window) {
    // Enable keyboard support
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
}


bool CloseWindowIsNotRequired(GLFWwindow *window) {
    bool escape_key_pressed = (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS);
    bool window_should_close = (glfwWindowShouldClose(window) == 0);
    return escape_key_pressed && window_should_close;
}


int main() {
    GLFWwindow * window;

    if( !glfwInit() ) {
        std::cout << "Error GLFW initialize." << std::endl;
        return -1;
    }

    ChangeGlfwWindowParams();

    window = glfwCreateWindow(window_width, window_height, "Lesson 1 – Open a window", NULL, NULL);
    if (window == NULL) {
        std::cout << "Can't create GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if ( glewInit() != GLEW_OK ) {
        std::cout << "Error GLEWn initialize." << std::endl;
        return -1;
    }

    PrepareGlfwWindow(window);

    // Redraw window
    while(CloseWindowIsNotRequired(window)) {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}

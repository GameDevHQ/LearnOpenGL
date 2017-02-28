#ifndef GLFW_WINDOW_H
#define GLFW_WINDOW_H
#include <string>
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

static const int WINDOW_WIDTH = 800;
static const int WINDOW_HEIGHT = 600;
static const std::string DEFAULT_WINDOW_NAME = "OpenGL tutorial";


class Window
{
public:
    /**
     * Constructor.
     * @param width Window width.
     * @param height Window
     */
    Window(int width = WINDOW_WIDTH, int height = WINDOW_HEIGHT, std::string name = DEFAULT_WINDOW_NAME);

    /**
     * Initialize OpenGL context.
     */
    void Initialize();

    /**
     * Run the main loop for an application.
     */
    void Run();
private:
    /**
     * GLFW window instance.
     */
   GLFWwindow* window;
};
#endif

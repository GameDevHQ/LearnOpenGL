#ifndef GLFW_INPUT_H
#define GLFW_INPUT_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Util.h"


class Input
{
public:
    /**
  * Checks if a given key is currently pressed.
     * @param window GFLW window instance.
  * @param keycode The key to check.
  * @return True if the given key is currently pressed.
  */
     static bool IsKeyPressed(GLFWwindow* window, KEYBOARD_KEY keycode);
};
#endif

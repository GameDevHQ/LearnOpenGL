#include "Input.h"


bool Input::IsKeyPressed(GLFWwindow* window, KEYBOARD_KEY keycode)
{
    switch (keycode)
    {
        case KEYBOARD_KEY::ESC:
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
            {
                return true;
            }
            break;
    }

    return false;
}

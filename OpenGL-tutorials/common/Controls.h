#ifndef CONTROLS_H
#define CONTROLS_H
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

static const float FOV = 45.f;
static const float ASPECT_RATIO = 4.f / 3.f;
static const float Z_NEAR = 0.1f;
static const float Z_FAR = 100.f;
static const float SPEED = 3.0f;  // 3 units / second
static const float MOUSE_SPEED = 0.005f;

void computeMatricesFromInputs(GLFWwindow* window);
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();
#endif

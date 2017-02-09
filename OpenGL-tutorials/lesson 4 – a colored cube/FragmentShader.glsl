#version 330 core

// Interpolated values from the vertex shaders
in vec3 fragmentColor;

out vec3 color; // output color of fragment

void main() {
    // Set white color
    color = fragmentColor;
}

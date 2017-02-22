#include "Window.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Input.h"
#include "Shader.h"
#include "Texture.h"
#include "Controls.h"
#include "ObjLoader.h"
#include "VBOIndexer.h"
#include "Text2d.h"


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
            "../lesson 11 – 2d text/VertexShader.glsl",
            "../lesson 11 – 2d text/FragmentShader.glsl"
    );

    // Get a handle for our "MVP" uniform
    GLint MatrixID = glGetUniformLocation(programID, "MVP");
    GLint ViewMatrixID = glGetUniformLocation(programID, "V");
    GLint ModelMatrixID = glGetUniformLocation(programID, "M");

    // Load a texture...
    GLuint texture = loadDDS("../resources/suzanne_uvmap.dds");

    // Get a handle for our "myTextureSampler" uniform
    GLint textureID  = glGetUniformLocation(programID, "myTextureSampler");

    // Get a handle for our "LightPosition" uniform
    GLint lightID = glGetUniformLocation(programID, "LightPosition_worldspace");

    // Read our .obj file
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    bool res = loadOBJ("../resources/suzanne.obj", vertices, uvs, normals);

    std::vector<unsigned short> indices;
    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec2> indexed_uvs;
    std::vector<glm::vec3> indexed_normals;
    indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);

    // Load it into a VBO
    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

    GLuint uvBuffer;
    glGenBuffers(1, &uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

    GLuint normalBuffer;
    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

    GLuint elementBuffer;
    glGenBuffers(1, &elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);

    // Initialize our little text library with the Holstein font
    initText2D(
            "../lesson 11 – 2d text/TextTexture.dds",
            "../lesson 11 – 2d text/TextVertexShader.glsl",
            "../lesson 11 – 2d text/TextFragmentShader.glsl",
            "myTextureSampler"
    );

    // Enable depth test.
    glEnable(GL_DEPTH_TEST);

    // Accept fragment if it closer to the camera than the former one.
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera.
    glEnable(GL_CULL_FACE);

    // For speed computation
    double lastTime = glfwGetTime();
    int nbFrames = 0;

    do
    {
        // Measure speed
        double currentTime = glfwGetTime();
        nbFrames++;
        if (currentTime - lastTime >= 1.0)
        {
            std::cout << 1000.0/double(nbFrames) << " ms/frame" << std::endl;
            nbFrames = 0;
            lastTime += 1.0;
        }

        // Clear the screen.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader.
        glUseProgram(programID);

        computeMatricesFromInputs(window);
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();
        glm::mat4 ModelMatrix = glm::mat4(1.0);
        glm::mat4 mvpView = ProjectionMatrix * ViewMatrix * ModelMatrix;

        // Send our transformation to the currently bound shader, in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvpView[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

        glm::vec3 lightPos = glm::vec3(4, 4, 4);
        glUniform3f(lightID, lightPos.x, lightPos.y, lightPos.z);

        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        // Set our "myTextureSampler" sampler to user Texture Unit 0
        glUniform1i(textureID, 0);

        // 1st attribute buffer: vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // 2nd attribute buffer: colors
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // 3rd attribute buffer : normals
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);

        // Draw the triangles.
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

        char text[256];
        sprintf(text,"%.2f sec", glfwGetTime());
        printText2D(text, 10, 500, 24);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    while (Input::IsKeyPressed(window, KEYBOARD_KEY::ESC) && glfwWindowShouldClose(window) == 0);

    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &uvBuffer);
    glDeleteBuffers(1, &normalBuffer);
    glDeleteProgram(programID);
    glDeleteTextures(1, &texture);
    glDeleteVertexArrays(1, &vertexArrayID);

    // Delete the text's VBO, the shader and the texture
    cleanupText2D();

    glfwTerminate();
}

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

    // Hide the mouse and enable unlimited movement.
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Clear the screen.
    glClearColor(0.f, 0.f, 0.f, 0.f);

    // Get an actual window size.
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // We would expect width and height to be 1024 and 768
    int windowWidth = 1024;
    int windowHeight = 768;
    // But on MacOS X with a retina screen it'll be 1024*2 and 768*2, so we get the actual framebuffer size.
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

    // Create Vertex Array Object (VAO).
    GLuint vertexArrayID;
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint depthProgramID = LoadShaders(
            "../lesson 16 – shadow mapping/VertexShader.glsl",
            "../lesson 16 – shadow mapping/FragmentShader.glsl"
    );

    // Get a handle for our "MVP" uniform
    GLint depthMatrixID = glGetUniformLocation(depthProgramID, "depthMVP");

    // Load the texture
    GLuint Texture = loadDDS("../lesson 16 – shadow mapping/uvmap.dds");

    // Read our .obj file
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    bool res = loadOBJ("../lesson 16 – shadow mapping/room.obj", vertices, uvs, normals);

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

    // Generate a buffer for the indices as well
    GLuint elementBuffer;
    glGenBuffers(1, &elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

    // -------------------
    //  Render to Texture
    // -------------------

    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    GLuint FramebufferName = 0;
    glGenFramebuffers(1, &FramebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

    // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
    GLuint depthTexture;
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, 1024, 1024, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

    // No color output in the bound framebuffer, only depth.
    glDrawBuffer(GL_NONE);

    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        return;
    }

    // -------------------
    // The quad's FBO. Used only for visualizing the shadowmap.
    static const GLfloat g_quad_vertex_buffer_data[] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f,  1.0f, 0.0f,
    };

    GLuint quad_vertexbuffer;
    glGenBuffers(1, &quad_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

    // Create and compile our GLSL program from the shaders
    GLuint quad_programID = LoadShaders(
            "../lesson 16 – shadow mapping/Passthrough.glsl",
            "../lesson 16 – shadow mapping/SimpleTexture.glsl"
    );
    GLint texID = glGetUniformLocation(quad_programID, "texture_img");

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders(
            "../lesson 16 – shadow mapping/ShadowMappingVertex.glsl",
            "../lesson 16 – shadow mapping/ShadowMappingFragment.glsl"
    );

    // Get a handle for our "myTextureSampler" uniform
    GLint textureID  = glGetUniformLocation(programID, "myTextureSampler");

    // Get a handle for our "MVP" uniform
    GLint MatrixID = glGetUniformLocation(programID, "MVP");
    GLint ViewMatrixID = glGetUniformLocation(programID, "V");
    GLint ModelMatrixID = glGetUniformLocation(programID, "M");
    GLint DepthBiasID = glGetUniformLocation(programID, "DepthBiasMVP");
    GLint ShadowMapID = glGetUniformLocation(programID, "shadowMap");

    // Get a handle for our "LightPosition" uniform
    GLint lightInvDirID = glGetUniformLocation(programID, "LightInvDirection_worldspace");

    // -------------------
    // Enable depth test.
    glEnable(GL_DEPTH_TEST);

    // Accept fragment if it closer to the camera than the former one.
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera.
    glEnable(GL_CULL_FACE);

    do
    {
        // Render to our framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
        glViewport(0,0,1024,1024); // Render on the whole framebuffer, complete from the lower left corner to the upper right

        // We don't use bias in the shader, but instead we draw back faces,
        // which are already separated from the front faces by a small distance
        // (if your geometry is made this way)
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(depthProgramID);

        glm::vec3 lightInvDir = glm::vec3(0.5f,2,2);

        // Compute the MVP matrix from the light's point of view
        glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10,10,-10,10,-10,20);
        glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0,0,0), glm::vec3(0,1,0));
        // or, for spot light :
        //glm::vec3 lightPos(5, 20, 20);
        //glm::mat4 depthProjectionMatrix = glm::perspective<float>(45.0f, 1.0f, 2.0f, 50.0f);
        //glm::mat4 depthViewMatrix = glm::lookAt(lightPos, lightPos-lightInvDir, glm::vec3(0,1,0));

        glm::mat4 depthModelMatrix = glm::mat4(1.0);
        glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &depthMVP[0][0]);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);

        // Draw the triangles.
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);

        glDisableVertexAttribArray(0);

        // Render to the screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0,0,windowWidth,windowHeight); // Render on the whole framebuffer, complete from the lower left corner to the upper right

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);

        // Compute the MVP matrix from keyboard and mouse input
        computeMatricesFromInputs(window);
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();
        //ViewMatrix = glm::lookAt(glm::vec3(14,6,4), glm::vec3(0,1,0), glm::vec3(0,1,0));
        glm::mat4 ModelMatrix = glm::mat4(1.0);
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

        glm::mat4 biasMatrix(
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0
        );

        glm::mat4 depthBiasMVP = biasMatrix*depthMVP;

        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
        glUniformMatrix4fv(DepthBiasID, 1, GL_FALSE, &depthBiasMVP[0][0]);

        glUniform3f(lightInvDirID, lightInvDir.x, lightInvDir.y, lightInvDir.z);

        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);
        // Set our "myTextureSampler" sampler to user Texture Unit 0
        glUniform1i(textureID, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        glUniform1i(ShadowMapID, 1);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // 2nd attribute buffer : UVs
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

        // Optionally render the shadowmap (for debug only)
        // Render only on a corner of the window (or we we won't see the real rendering...)
        glViewport(0, 0, 512, 512);

        // Use our shader
        glUseProgram(quad_programID);

        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        // Set our "renderedTexture" sampler to user Texture Unit 0
        glUniform1i(texID, 0);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // Draw the triangle.
        glDisableVertexAttribArray(0);

        // Swap buffers.
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    while (Input::IsKeyPressed(window, KEYBOARD_KEY::ESC) && glfwWindowShouldClose(window) == 0);

    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &uvBuffer);
    glDeleteBuffers(1, &normalBuffer);
    glDeleteBuffers(1, &elementBuffer);
    glDeleteProgram(programID);
    glDeleteProgram(depthProgramID);
    glDeleteProgram(quad_programID);
    glDeleteTextures(1, &Texture);

    glDeleteFramebuffers(1, &FramebufferName);
    glDeleteTextures(1, &depthTexture);
    glDeleteBuffers(1, &quad_vertexbuffer);
    glDeleteVertexArrays(1, &vertexArrayID);

    glfwTerminate();
}

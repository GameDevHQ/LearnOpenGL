#include <iostream>
#include <GL/glew.h>

#include "utils.h"
#include "shader.h"


GLuint LoadShaders(const char * vertex_filepath, const char * fragment_filepath) {
    GLuint ProgramID;
    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Create shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Load data from file
    std::string VertextShaderCode = GetShaderCodeFromFile(vertex_filepath);
    std::string FragmentShaderCode = GetShaderCodeFromFile(fragment_filepath);

    // Compile and check vertex/fragment shaders
    CompileShader(VertexShaderID, vertex_filepath, VertextShaderCode);
    CheckCompiledShader(VertexShaderID, &Result, &InfoLogLength);
    CompileShader(FragmentShaderID, fragment_filepath, FragmentShaderCode);
    CheckCompiledShader(FragmentShaderID, &Result, &InfoLogLength);

    // Create shader program and link compiled shader with her
    std::cout << "Create shader program..." << std::endl;
    ProgramID = glCreateProgram();
    AttachVertexAndFragmentShaders(ProgramID, VertexShaderID, FragmentShaderID);

    // Check shader program
    CheckProgram(ProgramID, &Result, &InfoLogLength);

    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}

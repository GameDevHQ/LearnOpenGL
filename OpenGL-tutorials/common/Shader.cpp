#include "Shader.h"
#include <iostream>


std::string GetShaderCodeFromFile(const char * filepath) {
    std::string ShaderCode = "";
    std::ifstream ShaderStream(filepath, std::ios::in);

    // Just read all file line-by-line
    if(ShaderStream.is_open()) {
        std::string line;
        while (getline(ShaderStream, line))
            ShaderCode += "\n" + line;
        ShaderStream.close();
    }
    else {
        std::cout << "Error: file " << std::string(filepath) << " not found." << std::endl;
    }

    return ShaderCode;
}


void CompileShader(GLuint shader_id, const char * filepath, const std::string &shader_code) {
    char const * SourcePointer = shader_code.c_str();

    std::cout << "Compile shader: " << std::string(filepath) << std::endl;
    glShaderSource(shader_id, 1, &SourcePointer, NULL);
    glCompileShader(shader_id);
}


void CheckCompiledShader(GLuint shader_id, GLint * result, int * info_log_length) {
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, result);
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, info_log_length);
    if ( *info_log_length > 0 ) {
        std::vector<char> ShaderErrorMessage(*info_log_length);
        glGetShaderInfoLog(shader_id, *info_log_length, NULL, &ShaderErrorMessage[0]);
        std::string error_string(ShaderErrorMessage.begin(), ShaderErrorMessage.end());
        std::cout << error_string << std::endl;
    }
}


void AttachVertexAndFragmentShaders(GLuint program_id, GLuint vertex_shader_id, GLuint fragment_shader_id) {
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    glLinkProgram(program_id);
};


void CheckProgram(GLuint program_id, GLint * result, int * info_log_length) {
    glGetProgramiv(program_id, GL_COMPILE_STATUS, result);
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, info_log_length);
    if ( *info_log_length > 0 ) {
        std::vector<char> ProgramErrorMessage(*info_log_length);
        glGetProgramInfoLog(program_id, *info_log_length, NULL, &ProgramErrorMessage[0]);
        std::string error_string(ProgramErrorMessage.begin(), ProgramErrorMessage.end());
        std::cout << error_string << std::endl;
    }
}


GLuint LoadShaders(const char * vertex_filepath, const char * fragment_filepath) {
    GLuint ProgramID;
    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Create shaders.
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Load data from file.
    std::string VertextShaderCode = GetShaderCodeFromFile(vertex_filepath);
    std::string FragmentShaderCode = GetShaderCodeFromFile(fragment_filepath);

    // Compile and check vertex/fragment shaders.
    CompileShader(VertexShaderID, vertex_filepath, VertextShaderCode);
    CheckCompiledShader(VertexShaderID, &Result, &InfoLogLength);
    CompileShader(FragmentShaderID, fragment_filepath, FragmentShaderCode);
    CheckCompiledShader(FragmentShaderID, &Result, &InfoLogLength);

    // Create shader program and link compiled shader with it.
    std::cout << "Create shader program..." << std::endl;
    ProgramID = glCreateProgram();
    AttachVertexAndFragmentShaders(ProgramID, VertexShaderID, FragmentShaderID);

    // Check shader program.
    CheckProgram(ProgramID, &Result, &InfoLogLength);

    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}

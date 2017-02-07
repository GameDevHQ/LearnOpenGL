#ifndef SHADER_H
#define SHADER_H
#include <fstream>
#include <vector>
#include <GL/glew.h>

std::string GetShaderCodeFromFile(const char * filepath);
void CompileShader(GLuint shader_id, const char * filepath, const std::string &shader_code);
void CheckCompiledShader(GLuint shader_id, GLint * result, int * info_log_length);
void AttachVertexAndFragmentShaders(GLuint ProgramID, GLuint VertexShaderID, GLuint FragmentShaderID);
void CheckProgram(GLuint program_id, GLint * result, int * info_log_length);
GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);

#endif

#ifndef UTILS_H
#define UTILS_H

std::string GetShaderCodeFromFile(const char * filepath);
void CompileShader(GLuint shader_id, const char * filepath, const std::string &shader_code);
void CheckCompiledShader(GLuint shader_id, GLint * result, int * info_log_length);
void AttachVertexAndFragmentShaders(GLuint ProgramID, GLuint VertexShaderID, GLuint FragmentShaderID);
void CheckProgram(GLuint program_id, GLint * result, int * info_log_length);

#endif

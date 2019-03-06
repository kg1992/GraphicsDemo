/*
    ShaderPrograms.h

    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    Dependencies :
        OpenGL - shader, program related codes are used
        glm - vector, matrice data types are used.

    ShaderProgram class definition.
*/
#ifndef SHADER_PROGRAM_H_
#define SHADER_PROGRAM_H_

#include <string>
#include <glad.h>
#include <glm/glm.hpp>

class ShaderProgram
{
public:
    bool Init(const char* name, const char* vertexShaderFilename, const char* fragmentShaderFilename);

    bool InitBySource(const char* name, const char* vertexShaderSource, const char* fragmentShaderSource);

    const std::string& GetName()
    {
        return m_name;
    }

    void SetName(const std::string& name)
    {
        m_name = name;
    }

    // wrapper to glUseProgram method
    void Use();

    void SendUniform(const char* name, float x);

    void SendUniform(const char* name, float x, float y);

    void SendUniform(const char* name, float x, float y, float z);

    void SendUniform(const char* name, float x, float y, float z, float w);

    void SendUniform3fv(const char* name, int count, float* fv);

    void SendUniform4fv(const char* name, int count, float *fv);

    void SendUniform(const char* name, int i);

    void SendUniform(const char* name, const glm::vec3& v);

    void SendUniform(const char* name, const glm::vec4& v);

    void SendUniformSubroutine(GLenum shaderType, const char* name);

    void SendUniform(const char* name, int count, bool transpose, const glm::mat3& m);

    void SendUniform(const char* name, int count, bool transpose, const glm::mat4& m);

    bool TrySendUniform(const char* name, int count, bool transpose, const glm::mat3& m);

    bool TrySendUniform(const char* name, int count, bool transpose, const glm::mat4& m);

    void GetProgramiv(GLenum pname, GLint& parameter);

    // GL_TRUE if program is currently flagged for deletion, and GL_FALSE otherwise.
    GLboolean GetDeleteStatus();

    // GL_TRUE if the last link operation on program was successful, and GL_FALSE otherwise.
    GLboolean GetLinkStatus();

    // GL_TRUE or if the last validation operation on program was successful, and GL_FALSE otherwise.
    GLboolean GetValidateStatus();

    // the number of characters in the information log for program including the null termination character(i.e., the size of the character buffer required to store the information log).If program has no information log, a value of 0 is returned.
    GLint GetInfoLogLength();

    // the number of shader objects attached to program.
    GLint GetAttachedShaders();

    // the number of active attribute variables for program.
    GLint GetActiveAttributes();

    // the length of the longest active attribute name for program, including the null termination character(i.e., the size of the character buffer required to store the longest attribute name).If no active attributes exist, 0 is returned.
    GLint GetActiveAttributeMaxLength();

    // the number of active uniform variables for program.
    GLint GetActiveUniformsCount();

    // the length of the longest active uniform variable name for program, including the null termination character(i.e., the size of the character buffer required to store the longest uniform variable name).If no active uniform variables exist, 0 is returned.
    GLint GetActiveUniformMaxLength();

    GLuint Handle() { return m_program; }

private:
    std::string m_name;

    GLuint m_program;

    GLint GetUniformLocation(const char* name);

    static std::string GetShaderInfoLog(GLuint shader);

    static std::string GetProgramInfoLog(GLuint program);

    static GLuint CompileShaderFromSourceFile(GLenum type, const std::string& fileName);

    static GLuint CompileShaderFromSource(GLenum type, const char* source);
};

#endif

#ifndef SHADER_PROGRAMS_H_
#define SHADER_PROGRAMS_H_

#include <glad.h>
#include <glm/glm.hpp>

class ShaderProgram
{
public:
    bool Init(const char* vertexShaderFilename, const char* fragmentShaderFilename);

    void Use();

    void SendUniform(const char* name, float x);

    void SendUniform(const char* name, float x, float y);

    void SendUniform(const char* name, float x, float y, float z);

    void SendUniform(const char* name, float x, float y, float z, float w);

    void SendUniform3fv(const char* name, int count, float* fv);

    void SendUniform4fv(const char* name, int count, float *fv);

    void SendUniform(const char* name, int i);

    void SendUniform(const char* name, const glm::vec4& v);

    void SendUniformSubroutine(GLenum shaderType, const char* name);

    void SendUniform(const char* name, int count, bool transpose, const glm::mat3& m);

    void SendUniform(const char* name, int count, bool transpose, const glm::mat4& m);

    GLuint Name() { return m_program; }

private:
    GLuint m_program;

    GLuint GetUniformLocation(const char* name);

    static std::string GetShaderInfoLog(GLuint shader);

    static std::string GetProgramInfoLog(GLuint program);

    static GLuint CompileShaderFromSourceFile(GLenum type, const std::string& fileName);

    static GLuint PrepareShaderProgram(const char* vs, const char* fs);
};

class ShaderPrograms
{
public:
    static ShaderProgram s_basic;
    static ShaderProgram s_position;
    static ShaderProgram s_uv;
    static ShaderProgram s_normal;
    static ShaderProgram s_plane;

    static void Init();
};

#endif

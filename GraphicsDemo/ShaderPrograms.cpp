/*
    ShaderPrograms.cpp

    basic vertex shader used for rendering most of the objects in GraphicsDemo project.
*/
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <map>
#include <glad.h>
#include "ShaderPrograms.h"
#include "Errors.h"

GLuint ShaderProgram::GetUniformLocation(const char* name)
{
    GLint location = glGetUniformLocation(m_program, name);
    GET_AND_HANDLE_GL_ERROR();
    return location;
}

std::string ShaderProgram::GetShaderInfoLog(GLuint shader)
{
    GLsizei bufSize;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);
    GET_AND_HANDLE_GL_ERROR();

    std::vector<GLchar> buf;
    buf.resize(bufSize + 1);

    glGetShaderInfoLog(shader, bufSize, nullptr, (GLchar*)buf.data());
    GET_AND_HANDLE_GL_ERROR();

    return std::string(buf.data());
}

std::string ShaderProgram::GetProgramInfoLog(GLuint program)
{
    GLsizei bufSize;

    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);
    GET_AND_HANDLE_GL_ERROR();

    std::vector<GLchar> buf;
    buf.resize(bufSize + 1);

    glGetProgramInfoLog(program, bufSize, nullptr, (GLchar*)buf.data());
    GET_AND_HANDLE_GL_ERROR();

    return std::string(buf.data());
}

GLuint ShaderProgram::CompileShaderFromSourceFile(GLenum type, const std::string& filename)
{
    std::ifstream fin(filename);
    if (!fin)
    {
        std::cerr << __FUNCTION__ << " : Error. Failed to open '" << filename << "'" << std::endl;
        return 0;
    }

    std::vector<GLchar> source((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());

    GLuint shader = glCreateShader(type);
    GET_AND_HANDLE_GL_ERROR();

    GLchar* pSource = source.data();
    GLint sourceSize = static_cast<GLint>(source.size());

    glShaderSource(shader, 1, &pSource, &sourceSize);
    GET_AND_HANDLE_GL_ERROR();

    glCompileShader(shader);
    GET_AND_HANDLE_GL_ERROR();

    // Test If Success
    GLint success = GL_TRUE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    GET_AND_HANDLE_GL_ERROR();

    if (success == GL_FALSE)
    {
        return 0;
    }

    std::string infoLog = GetShaderInfoLog(shader);
    if (infoLog.size() != 0)
    {
        std::cerr << __FUNCTION__ << " : Failed to compile shader '"
                  << filename << "'. Info log : " << std::endl
                  << infoLog << std::endl;
    }

    return shader;
}

bool ShaderProgram::Init(const char* vertexShaderFilename, const char* fragmentShaderFilename)
{
    GLuint vertexShader = CompileShaderFromSourceFile(GL_VERTEX_SHADER, vertexShaderFilename);
    GLuint fragmentShader = CompileShaderFromSourceFile(GL_FRAGMENT_SHADER, fragmentShaderFilename);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    GET_AND_HANDLE_GL_ERROR();
    glAttachShader(program, fragmentShader);
    GET_AND_HANDLE_GL_ERROR();
    glLinkProgram(program);
    GET_AND_HANDLE_GL_ERROR();

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    GET_AND_HANDLE_GL_ERROR();
    if (success == GL_FALSE) {
        std::cerr << __FUNCTION__ << " : Faield to link program. Log : " << std::endl
                  << GetProgramInfoLog(program) << std::endl;
        return false;
    }

    glDeleteShader(vertexShader);
    GET_AND_HANDLE_GL_ERROR();
    glDeleteShader(fragmentShader);
    GET_AND_HANDLE_GL_ERROR();

    m_program = program;
    return true;
}

void ShaderProgram::Use()
{
    glUseProgram(m_program);
    GET_AND_HANDLE_GL_ERROR();
}

void ShaderProgram::SendUniform(const char* name, float x)
{
    glUniform1f(GetUniformLocation(name), x);
    GET_AND_HANDLE_GL_ERROR();
}

void ShaderProgram::SendUniform(const char* name, float x, float y)
{
    glUniform2f(GetUniformLocation(name), x, y);
    GET_AND_HANDLE_GL_ERROR();
}

void ShaderProgram::SendUniform(const char* name, float x, float y, float z)
{
    glUniform3f(GetUniformLocation(name), x, y, z);
    GET_AND_HANDLE_GL_ERROR();
}

void ShaderProgram::SendUniform(const char* name, float x, float y, float z, float w)
{
    glUniform4f(GetUniformLocation(name), x, y, z, w);
    GET_AND_HANDLE_GL_ERROR();
}

void ShaderProgram::SendUniform3fv(const char* name, int count, float* fv)
{
    glUniform3fv(GetUniformLocation(name), count, fv);
    GET_AND_HANDLE_GL_ERROR();
}

void ShaderProgram::SendUniform4fv(const char* name, int count, float *fv)
{
    glUniform4fv(GetUniformLocation(name), count, fv);
    GET_AND_HANDLE_GL_ERROR();
}

void ShaderProgram::SendUniform(const char* name, int i)
{
    glUniform1i(GetUniformLocation(name), i);
    GET_AND_HANDLE_GL_ERROR();
}

void ShaderProgram::SendUniform(const char* name, const glm::vec4& v)
{
    SendUniform(name, v.x, v.y, v.z, v.w);
    GET_AND_HANDLE_GL_ERROR();
}

void ShaderProgram::SendUniformSubroutine(GLenum shaderType, const char* name)
{
    GLuint adsIndex = glGetSubroutineIndex(m_program, shaderType, name);
    GET_AND_HANDLE_GL_ERROR();
    glUniformSubroutinesuiv(m_program, 1, &adsIndex);
    GET_AND_HANDLE_GL_ERROR();
}

void ShaderProgram::SendUniform(const char* name, int count, bool transpose, const glm::mat3& m)
{
    glUniformMatrix3fv(GetUniformLocation(name), count, transpose, &m[0][0]);
    GET_AND_HANDLE_GL_ERROR();
}

void ShaderProgram::SendUniform(const char* name, int count, bool transpose, const glm::mat4& m)
{
    glUniformMatrix4fv(GetUniformLocation(name), count, transpose, &m[0][0]);
    GET_AND_HANDLE_GL_ERROR();
}

////////////////////////////////////////////////////////////////////////////////

void ShaderPrograms::Init()
{
    s_basic.Init("basic.vert.glsl", "basic.frag.glsl");
    s_position.Init("basic.vert.glsl", "position.frag.glsl");
    s_uv.Init("basic.vert.glsl", "uv.frag.glsl");
    s_normal.Init("basic.vert.glsl", "normal.frag.glsl");
    s_plane.Init("billboard.vert.glsl", "uv.frag.glsl");
}

ShaderProgram ShaderPrograms::s_basic;
ShaderProgram ShaderPrograms::s_position;
ShaderProgram ShaderPrograms::s_uv;
ShaderProgram ShaderPrograms::s_normal;
ShaderProgram ShaderPrograms::s_plane;

#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <map>
#include <glad.h>
#include "ShaderPrograms.h"

std::string GetShaderInfoLog(GLuint shader)
{
    GLsizei bufSize;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);

    std::vector<GLchar> buf;
    buf.resize(bufSize + 1);

    // Get InfoLog
    glGetShaderInfoLog(shader, bufSize, nullptr, (GLchar*)buf.data());

    // return buffer
    return std::string(buf.data());
}

std::string GetProgramInfoLog(GLuint program)
{
    GLsizei bufSize;

    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);

    std::vector<GLchar> buf;
    buf.resize(bufSize + 1);

    // Get InfoLog
    glGetProgramInfoLog(program, bufSize, nullptr, (GLchar*)buf.data());

    // return buffer
    return std::string(buf.data());
}

GLuint CompileShaderFromSourceFile(GLenum type, const std::string& fileName)
{
    GLuint shader = glCreateShader(type);
    std::ifstream fin(fileName);
    std::vector<GLchar> source((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
    GLchar* pSource = source.data();
    GLint sourceSize = static_cast<GLint>(source.size());
    glShaderSource(shader, 1, &pSource, &sourceSize);
    glCompileShader(shader);
    std::string infoLog = GetShaderInfoLog(shader);
    if (infoLog.size() != 0) {
        std::cout << infoLog << std::endl;
    }
    return shader;
}

GLuint PrepareShaderProgram(const char* vs, const char* fs)
{
    GLuint vertexShader = CompileShaderFromSourceFile(GL_VERTEX_SHADER, vs);
    GLuint fragmentShader = CompileShaderFromSourceFile(GL_FRAGMENT_SHADER, fs);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    std::string programInfoLog = GetProgramInfoLog(program);
    if (programInfoLog.size() != 0) {

        std::cout << " * Program Info Log : " << std::endl
            << GetProgramInfoLog(program) << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

GLuint ShaderPrograms::s_basic;
GLuint ShaderPrograms::s_position;
GLuint ShaderPrograms::s_uv;
GLuint ShaderPrograms::s_normal;

void ShaderPrograms::Init()
{
    s_basic = PrepareShaderProgram("basic.vert.glsl", "basic.frag.glsl");
    s_position = PrepareShaderProgram("basic.vert.glsl", "position.frag.glsl");
    s_uv = PrepareShaderProgram("basic.vert.glsl", "uv.frag.glsl");
    s_normal = PrepareShaderProgram("basic.vert.glsl", "normal.frag.glsl");
}

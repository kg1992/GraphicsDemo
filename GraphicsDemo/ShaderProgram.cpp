/*
    ShaderProgram.cpp

    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    Dependencies :
        OpenGL - shader, program related codes are used
        glm - vector, matrice data types are used.

    ShaderProgram class definition.
*/
#include "Common.h"
#include "ShaderProgram.h"
#include "Errors.h"

GLint ShaderProgram::GetUniformLocation(const char* name)
{
    GLint location = glGetUniformLocation(m_program, name);
    GET_AND_HANDLE_GL_ERROR();
    if (location == -1) DebugBreak();
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
    source.push_back('\0');
    GLchar* pSource = source.data();
    return CompileShaderFromSource(type, pSource);
}

GLuint ShaderProgram::CompileShaderFromSource(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);
    GET_AND_HANDLE_GL_ERROR();

    glShaderSource(shader, 1, &source, 0);
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
        std::cerr << __FUNCTION__ << " : Failed to compile shader. Info log : " << std::endl
            << infoLog << std::endl;
    }

    return shader;
}

bool ShaderProgram::Init(const char* name, const char* vertexShaderFilename, const char* fragmentShaderFilename)
{
    GLuint vertexShader = CompileShaderFromSourceFile(GL_VERTEX_SHADER, vertexShaderFilename);
    GLuint fragmentShader = CompileShaderFromSourceFile(GL_FRAGMENT_SHADER, fragmentShaderFilename);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        std::cerr << __FUNCTION__ << " : Faield to link program. Log : " << std::endl
                  << GetProgramInfoLog(program) << std::endl;
        return false;
    }

    glDeleteShader(vertexShader);
    GET_AND_HANDLE_GL_ERROR();
    glDeleteShader(fragmentShader);
    GET_AND_HANDLE_GL_ERROR();

    m_name = name;
    m_program = program;
    return true;
}

bool ShaderProgram::InitBySource(const char* name, const char* vertexShaderSource, const char* fragmentShaderSource)
{
    GLuint vertexShader = CompileShaderFromSource(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = CompileShaderFromSource(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        std::cerr << __FUNCTION__ << " : Faield to link program. Log : " << std::endl
            << GetProgramInfoLog(program) << std::endl;
        return false;
    }

    glDeleteShader(vertexShader);
    GET_AND_HANDLE_GL_ERROR();
    glDeleteShader(fragmentShader);
    GET_AND_HANDLE_GL_ERROR();

    m_name = name;
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
    GLint location = GetUniformLocation(name);
    glUniform1f(location, x);
    GET_AND_HANDLE_GL_ERROR();
}

bool ShaderProgram::TrySendUniform(const char * name, float x)
{
    GLint location = glGetUniformLocation(m_program, name);
    if (location == -1)
        return false;
    glUniform1f(location, x);
    if (glGetError() != GL_NO_ERROR)
        return false;
    return true;
}

void ShaderProgram::SendUniform(const char* name, float x, float y)
{
    GLint location = GetUniformLocation(name);
    glUniform2f(location, x, y);
    GET_AND_HANDLE_GL_ERROR();
}

bool ShaderProgram::TrySendUniform(const char * name, float x, float y)
{
    GLint location = glGetUniformLocation(m_program, name);
    if (location == -1)
        return false;
    glUniform2f(location, x, y);
    if (glGetError() != GL_NO_ERROR)
        return false;
    return true;
}

void ShaderProgram::SendUniform(const char* name, float x, float y, float z)
{
    GLint location = GetUniformLocation(name);
    glUniform3f(location, x, y, z);
    GET_AND_HANDLE_GL_ERROR();
}

bool ShaderProgram::TrySendUniform(const char * name, float x, float y, float z)
{
    GLint location = glGetUniformLocation(m_program, name);
    if (location == -1)
        return false;
    glUniform3f(location, x, y, z);
    if (glGetError() != GL_NO_ERROR)
        return false;
    return true;
}

void ShaderProgram::SendUniform(const char* name, float x, float y, float z, float w)
{
    GLint location = GetUniformLocation(name);
    glUniform4f(location, x, y, z, w);
    GET_AND_HANDLE_GL_ERROR();
}

bool ShaderProgram::TrySendUniform(const char * name, float x, float y, float z, float w)
{
    GLint location = glGetUniformLocation(m_program, name);
    if (location == -1)
        return false;
    glUniform4f(location, x, y, z, w);
    if (glGetError() != GL_NO_ERROR)
        return false;
    return true;
}

void ShaderProgram::SendUniform3fv(const char* name, int count, const float* fv)
{
    GLint location = GetUniformLocation(name);
    glUniform3fv(location, count, fv);
    GET_AND_HANDLE_GL_ERROR();
}

bool ShaderProgram::TrySendUniform3fv(const char * name, int count, const float * fv)
{
    GLint location = glGetUniformLocation(m_program, name);
    if (location == -1)
        return false;
    glUniform3fv(location, count, fv);
    if (glGetError() != GL_NO_ERROR)
        return false;
    return true;
}

void ShaderProgram::SendUniform4fv(const char* name, int count, const float* fv)
{
    GLint location = GetUniformLocation(name);
    glUniform4fv(location, count, fv);
    GET_AND_HANDLE_GL_ERROR();
}

bool ShaderProgram::TrySendUniform4fv(const char * name, int count, const float* fv)
{
    GLint location = glGetUniformLocation(m_program, name);
    if (location == -1)
        return false;
    glUniform4fv(location, count, fv);
    if (glGetError() != GL_NO_ERROR)
        return false;
    return true;
}

void ShaderProgram::SendUniform(const char* name, int i)
{
    GLint location = GetUniformLocation(name);
    glUniform1i(location, i);
    GET_AND_HANDLE_GL_ERROR();
}

bool ShaderProgram::TrySendUniform(const char * name, int i)
{
    GLint location = glGetUniformLocation(m_program, name);
    if (location == -1)
        return false;
    glUniform1i(location, i);
    if (glGetError() != GL_NO_ERROR)
        return false;
    return true;
}

void ShaderProgram::SendUniform(const char* name, unsigned int ui)
{
    GLint location = GetUniformLocation(name);
    glUniform1ui(location, ui);
    GET_AND_HANDLE_GL_ERROR();
}

bool ShaderProgram::TrySendUniform(const char * name, unsigned int ui)
{
    GLint location = glGetUniformLocation(m_program, name);
    if (location == -1)
        return false;
    glUniform1ui(location, ui);
    if (glGetError() != GL_NO_ERROR)
        return false;
    return true;
}

void ShaderProgram::SendUniform(const char* name, const glm::vec3& v)
{
    SendUniform(name, v.x, v.y, v.z);
}

bool ShaderProgram::TrySendUniform(const char * name, const glm::vec3 & v)
{
    return TrySendUniform(name, v.x, v.y, v.z);
}

void ShaderProgram::SendUniform(const char* name, const glm::vec4& v)
{
    SendUniform(name, v.x, v.y, v.z, v.w);
}

bool ShaderProgram::TrySendUniform(const char * name, const glm::vec4 & v)
{
    return TrySendUniform(name, v.x, v.y, v.z, v.w);
}

void ShaderProgram::SendUniformSubroutine(GLenum shaderType, const char* name)
{
    GLuint adsIndex = glGetSubroutineIndex(m_program, shaderType, name);
    GET_AND_HANDLE_GL_ERROR();
    glUniformSubroutinesuiv(m_program, 1, &adsIndex);
    GET_AND_HANDLE_GL_ERROR();
}

bool ShaderProgram::TrySendUniformSubroutine(GLenum shaderType, const char * name)
{
    GLuint adsIndex = glGetSubroutineIndex(m_program, shaderType, name);
    // Todo : check if this is correct answer
    if (adsIndex == -1)
        return false;
    GLint location = glGetUniformLocation(m_program, name);
    glUniformSubroutinesuiv(m_program, location, &adsIndex);
    if (glGetError() != GL_NO_ERROR)
        return false;
    return true;
}

void ShaderProgram::SendUniform(const char* name, int count, bool transpose, const glm::mat3& m)
{
    glUniformMatrix3fv(GetUniformLocation(name), count, transpose, &m[0][0]);
    GET_AND_HANDLE_GL_ERROR();
}

bool ShaderProgram::TrySendUniform(const char* name, int count, bool transpose, const glm::mat3& m)
{
    GLint location = glGetUniformLocation(m_program, name);
    if (location == -1)
        return false;
    glUniformMatrix3fv(location, count, transpose, &m[0][0]);
    if (glGetError() != GL_NO_ERROR)
        return false;
    return true;
}

void ShaderProgram::SendUniform(const char* name, int count, bool transpose, const glm::mat4& m)
{
    glUniformMatrix4fv(GetUniformLocation(name), count, transpose, &m[0][0]);
    GET_AND_HANDLE_GL_ERROR();
}

bool ShaderProgram::TrySendUniform(const char* name, int count, bool transpose, const glm::mat4& m)
{
    GLint location = glGetUniformLocation(m_program, name);
    if (location == -1)
        return false;
    glUniformMatrix4fv(location, count, transpose, &m[0][0]);
    if (glGetError() != GL_NO_ERROR)
        return false;
    return true;
}

void ShaderProgram::GetProgramiv(GLenum pname, GLint& parameter)
{
    glGetProgramiv(m_program, pname, &parameter);
    GET_AND_HANDLE_GL_ERROR();
}

// GL_TRUE if program is currently flagged for deletion, and GL_FALSE otherwise.

GLboolean ShaderProgram::GetDeleteStatus()
{
    GLint param;
    GetProgramiv(GL_DELETE_STATUS, param);
    return param;
}

// GL_TRUE if the last link operation on program was successful, and GL_FALSE otherwise.

GLboolean ShaderProgram::GetLinkStatus()
{
    GLint param;
    GetProgramiv(GL_LINK_STATUS, param);
    return param;
}

// GL_TRUE or if the last validation operation on program was successful, and GL_FALSE otherwise.

GLboolean ShaderProgram::GetValidateStatus()
{
    GLint param;
    GetProgramiv(GL_VALIDATE_STATUS, param);
    return param;
}

// the number of characters in the information log for program including the null termination character(i.e., the size of the character buffer required to store the information log).If program has no information log, a value of 0 is returned.

GLint ShaderProgram::GetInfoLogLength()
{
    GLint param;
    GetProgramiv(GL_INFO_LOG_LENGTH, param);
    return param;
}

// the number of shader objects attached to program.

GLint ShaderProgram::GetAttachedShaders()
{
    GLint param;
    GetProgramiv(GL_ATTACHED_SHADERS, param);
    return param;
}

// the number of active attribute variables for program.

GLint ShaderProgram::GetActiveAttributes()
{
    GLint param;
    GetProgramiv(GL_ACTIVE_ATTRIBUTES, param);
    return param;
}

// the length of the longest active attribute name for program, including the null termination character(i.e., the size of the character buffer required to store the longest attribute name).If no active attributes exist, 0 is returned.

GLint ShaderProgram::GetActiveAttributeMaxLength()
{
    GLint param;
    GetProgramiv(GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, param);
    return param;
}

// the number of active uniform variables for program.

GLint ShaderProgram::GetActiveUniformsCount()
{
    GLint param;
    GetProgramiv(GL_ACTIVE_UNIFORMS, param);
    return param;
}

// the length of the longest active uniform variable name for program, including the null termination character(i.e., the size of the character buffer required to store the longest uniform variable name).If no active uniform variables exist, 0 is returned.

GLint ShaderProgram::GetActiveUniformMaxLength()
{
    GLint param;
    GetProgramiv(GL_ACTIVE_UNIFORM_MAX_LENGTH, param);
    return param;
}

//GLint activeUniformCount = program.GetActiveUniformsCount();

//std::vector<std::string> names;

//for (int i = 0; i < activeUniformCount; i++)
//{
//    const int BufferSize = 0x100;
//    std::vector<GLchar> buffer(BufferSize, '\0');
//    GLuint length = BufferSize;
//    GLint varNameLength;
//    GLint varSize;
//    GLenum varType;
//    glGetActiveUniform(program.Handle(), i, BufferSize, &varNameLength, &varSize, &varType, buffer.data());
//    buffer.resize(length);
//    names.push_back(std::string(buffer.begin(), buffer.end()));
//}

////////////////////////////////////////////////////////////////////////////////

#include "Material.h"
#include "Errors.h"

Material::Material()
    : m_diffuseMap(0)
    , m_specularMap(0)
{
}

void Material::SetProgram(GLuint program)
{
    m_program = program;
}

GLuint Material::GetProgram()
{
    return m_program;
}

void Material::Fill(float * data, int width, int height)
{
    glGenTextures(1, &m_diffuseMap);
    GET_AND_HANDLE_GL_ERROR();

    glBindTexture(GL_TEXTURE_2D, m_diffuseMap);
    GET_AND_HANDLE_GL_ERROR();

    //glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, 256, 256);
    //GET_AND_HANDLE_GL_ERROR();

    //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 256, GL_RGBA, GL_FLOAT, data);
    //GET_AND_HANDLE_GL_ERROR();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    GET_AND_HANDLE_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    GET_AND_HANDLE_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    GET_AND_HANDLE_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GET_AND_HANDLE_GL_ERROR();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, data);
    GET_AND_HANDLE_GL_ERROR();
    glGenerateMipmap(GL_TEXTURE_2D);
    GET_AND_HANDLE_GL_ERROR();
}

void Material::Free()
{
    if (m_diffuseMap != 0)
    {
        glDeleteTextures(1, &m_diffuseMap);
        GET_AND_HANDLE_GL_ERROR();
    }
}

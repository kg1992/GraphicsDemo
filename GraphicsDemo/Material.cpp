/*
    Material.cpp

    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    Material class implementation
*/
#include "Common.h"
#include "Material.h"
#include "Errors.h"

Material::Material()
    : m_emissive()
    , m_ambient()
    , m_diffuse()
    , m_specular()
    , m_shininess(.0f)
{
}

void Material::Free()
{
    if (m_ambient.map != 0)
    {
        glDeleteTextures(1, &m_ambient.map);
        m_ambient.map = 0;
    }

    if (m_diffuse.map != 0)
    {
        glDeleteTextures(1, &m_diffuse.map);
        m_ambient.map = 0;
    }

    if (m_specular.map != 0)
    {
        glDeleteTextures(1, &m_specular.map);
        m_specular.map = 0;
    }

    if (m_emissive.map != 0)
    {
        glDeleteTextures(1, &m_emissive.map);
        m_emissive.map = 0;
    }
}

GLuint Material::GetEmissiveMap() { return m_emissive.map; }

void Material::SetEmissiveMap(GLuint map) { m_emissive.map = map; }

const glm::vec4& Material::GetEmissiveColor() { return m_emissive.color; }

void Material::SetEmissiveColor(const glm::vec4 & color) { m_emissive.color = color; }

void Material::SetEmissiveColor(float r, float g, float b, float a)
{
    m_ambient.color[0] = r;
    m_ambient.color[1] = g;
    m_ambient.color[2] = b;
    m_ambient.color[3] = a;
}

GLuint Material::GetAmbientMap() { return m_ambient.map; }

void Material::SetAmbientMap(GLuint map) { m_ambient.map = map; }

const glm::vec4& Material::GetAmbientColor() { return m_ambient.color; }

void Material::SetAmbientColor(const glm::vec4 & color) { m_ambient.color = color; }

void Material::SetAmbientColor(float r, float g, float b, float a)
{
    m_ambient.color[0] = r;
    m_ambient.color[1] = g;
    m_ambient.color[2] = b;
    m_ambient.color[3] = a;
}

GLuint Material::GetDiffuseMap() { return m_diffuse.map; }

void Material::SetDiffuseMap(GLuint map) { m_diffuse.map = map; }

const glm::vec4& Material::GetDiffuseColor() { return m_diffuse.color; }

void Material::SetDiffuseColor(const glm::vec4 & color)
{
    SetDiffuseColor(color.r, color.g, color.b, color.a);
}

void Material::SetDiffuseColor(float r, float g, float b, float a)
{
    m_diffuse.color[0] = r;
    m_diffuse.color[1] = g;
    m_diffuse.color[2] = b;
    m_diffuse.color[3] = a;
}

GLuint Material::GetSpecularMap() { return m_specular.map; }

void Material::SetSpecularMap(GLuint map) { m_specular.map = map; }

const glm::vec4& Material::GetSpecularColor() { return m_specular.color; }

void Material::SetSpecularColor(const glm::vec4 & color) { m_specular.color = color; }

void Material::SetSpecularColor(float r, float g, float b, float a)
{
    m_specular.color[0] = r;
    m_specular.color[1] = g;
    m_specular.color[2] = b;
    m_specular.color[3] = a;
}

GLfloat Material::GetShininess() { return m_shininess; }

void Material::SetShininess(GLfloat shininess) { m_shininess = shininess; }

Material& Material::GetDefaultMaterial()
{
    static Material s_defaultMaterial;
    s_defaultMaterial.SetAmbientColor(1.0f, .0f, .0f);
    s_defaultMaterial.SetDiffuseColor(1.0f, .0f, .0f);
    s_defaultMaterial.SetSpecularColor(1.0f, .0f, .0f);
    return s_defaultMaterial;
}

const std::string & Material::GetName()
{
    return m_name;
}

void Material::SetName(const std::string & name)
{
    m_name = name;
}

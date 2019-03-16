/*
    Material.cpp

    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    Material class implementation
*/
#include "Common.h"
#include "Material.h"
#include "Errors.h"
#include "Serialization.h"

Material::Material()
    : m_emissive()
    , m_ambient()
    , m_diffuse()
    , m_specular()
    , m_shininess(.0f)
    , m_normalMap(0)
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
    m_emissive.color[0] = r;
    m_emissive.color[1] = g;
    m_emissive.color[2] = b;
    m_emissive.color[3] = a;
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

const GLuint Material::GetNormalMap()
{
    return m_normalMap;
}

void Material::SetNormalMap(GLuint normalMap)
{
    m_normalMap = normalMap;
}

namespace {
    enum TextureMapTarget
    {
        TMT_EMISSIVE,
        TMT_AMBIENT,
        TMT_DIFFUSE,
        TMT_SPECULAR,
        TMT_NORMAL
    };

    const int PixelComponentCount = 4;

    void SerializeTextureObject(std::ostream& os, GLuint textureObject)
    {
        glBindTexture(GL_TEXTURE_2D, textureObject);
        GET_AND_HANDLE_GL_ERROR();

        GLint width;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        GET_AND_HANDLE_GL_ERROR();
        assert(width > 0);

        GLint height;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
        GET_AND_HANDLE_GL_ERROR();
        assert(height > 0);

        std::vector<char> buffer(sizeof(float) * width * height * PixelComponentCount);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, buffer.data());
        GET_AND_HANDLE_GL_ERROR();

        Serialization::Write(os, width);
        Serialization::Write(os, height);
        os.write(buffer.data(), buffer.size());
    }

    void DeserializeTextureObject(std::istream& is, GLuint& map)
    {
        int width, height;
        Serialization::Read(is, width);
        Serialization::Read(is, height);
        std::vector<char> buffer(width * height * PixelComponentCount * sizeof(float), '\0');
        is.read(buffer.data(), buffer.size());

        glGenTextures(1, &map);
        GET_AND_HANDLE_GL_ERROR();

        glBindTexture(GL_TEXTURE_2D, map);
        GET_AND_HANDLE_GL_ERROR();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GET_AND_HANDLE_GL_ERROR();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GET_AND_HANDLE_GL_ERROR();

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, buffer.data());
        GET_AND_HANDLE_GL_ERROR();
    }

    void SerializeMap(std::ostream& os, GLuint map)
    {
        int mapCount = map ? 1 : 0;
        Serialization::Write(os, mapCount);
        if (mapCount)
        {
            SerializeTextureObject(os, map);
        }
    }

    void DeserializeMap(std::istream& is, GLuint& map)
    {
        int mapCount;
        Serialization::Read(is, mapCount);
        if (mapCount)
        {
            DeserializeTextureObject(is, map);
        }
    }
}

void Material::Serialize(std::ostream & os)
{
    SerializeColorChannel(os, m_emissive);
    SerializeColorChannel(os, m_ambient);
    SerializeColorChannel(os, m_diffuse);
    SerializeColorChannel(os, m_specular);
    Serialization::Write(os, m_shininess);
    SerializeMap(os, m_normalMap);
}

void Material::Deserialize(std::istream& is)
{
    DeserializeColorChannel(is, m_emissive);
    DeserializeColorChannel(is, m_ambient);
    DeserializeColorChannel(is, m_diffuse);
    DeserializeColorChannel(is, m_specular);
    Serialization::Read(is, m_shininess);
    DeserializeMap(is, m_normalMap);
}

void Material::SerializeColorChannel(std::ostream& os, const Material::ColorChannel& cc)
{
    Serialization::Write(os, cc.color);
    SerializeMap(os, cc.map);
}

void Material::DeserializeColorChannel(std::istream& is, Material::ColorChannel& cc)
{
    Serialization::Read(is, cc.color);
    DeserializeMap(is, cc.map);
}
/*
    Material.h

    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    Material class definition
*/
#ifndef MATERIAL_H_
#define MATERIAL_H_

class Material
{
public:
    enum
    {
        COLOR_SIZE = 4
    };

    Material();

    void Free();

    GLuint GetEmissiveMap() { return m_emissive.map; }

    void SetEmissiveMap(GLuint map) { m_emissive.map = map; }

    GLfloat* GetEmissiveColor() { return m_emissive.color; }

    void SetEmissiveColor(const glm::vec4& color) { SetEmissiveColor(color.r, color.g, color.b, color.a); }

    void SetEmissiveColor(float r, float g, float b, float a = 1.0f)
    {
        m_ambient.color[0] = r;
        m_ambient.color[1] = g;
        m_ambient.color[2] = b;
        m_ambient.color[3] = a;
    }

    GLuint GetAmbientMap() { return m_ambient.map; }

    void SetAmbientMap(GLuint map) { m_ambient.map = map; }

    GLfloat* GetAmbientColor() { return m_ambient.color; }

    void SetAmbientColor(const glm::vec4& color) { SetAmbientColor(color.r, color.g, color.b, color.a); }

    void SetAmbientColor(float r, float g, float b, float a = 1.0f)
    {
        m_ambient.color[0] = r;
        m_ambient.color[1] = g;
        m_ambient.color[2] = b;
        m_ambient.color[3] = a;
    }

    GLuint GetDiffuseMap() { return m_diffuse.map; }

    void SetDiffuseMap(GLuint map) { m_diffuse.map = map; }

    GLfloat* GetDiffuseColor() { return m_diffuse.color; }

    void SetDiffuseColor(const glm::vec4& color)
    { 
        SetDiffuseColor(color.r, color.g, color.b, color.a);
    }

    void SetDiffuseColor(float r, float g, float b, float a = 1.0f)
    {
        m_diffuse.color[0] = r;
        m_diffuse.color[1] = g;
        m_diffuse.color[2] = b;
        m_diffuse.color[3] = a;
    }

    GLuint GetSpecularMap() { return m_specular.map; }

    void SetSpecularMap(GLuint map) { m_specular.map = map; }

    GLfloat* GetSpecularColor() { return m_specular.color; }

    void SetSpecularColor(const glm::vec4& color) { SetSpecularColor(color.r, color.g, color.b, color.a); }

    void SetSpecularColor(float r, float g, float b, float a = 1.0f)
    {
        m_specular.color[0] = r;
        m_specular.color[1] = g;
        m_specular.color[2] = b;
        m_specular.color[3] = a;
    }



    GLfloat GetShininess() { return m_shininess; }

    void SetShininess(GLfloat shininess) { m_shininess = shininess; }

    // Bake material properties.
    bool Initialize(const FbxSurfaceMaterial * pMaterial);

    static Material& GetDefaultMaterial();

private:
    struct ColorChannel
    {
        ColorChannel() : map(0)
        {
            color[0] = 0.0f;
            color[1] = 0.0f;
            color[2] = 0.0f;
            color[3] = 1.0f;
        }

        GLuint map;
        GLfloat color[COLOR_SIZE];
    };

    ColorChannel m_emissive;
    ColorChannel m_ambient;
    ColorChannel m_diffuse;
    ColorChannel m_specular;
    GLfloat m_shininess;
};

#endif

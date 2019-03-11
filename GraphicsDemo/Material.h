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
    Material();

    void Free();

    GLuint GetEmissiveMap();

    void SetEmissiveMap(GLuint map);

    const glm::vec4& GetEmissiveColor();

    void SetEmissiveColor(const glm::vec4& color);

    void SetEmissiveColor(float r, float g, float b, float a = 1.0f);

    GLuint GetAmbientMap();

    void SetAmbientMap(GLuint map);

    const glm::vec4& GetAmbientColor();

    void SetAmbientColor(const glm::vec4& color);

    void SetAmbientColor(float r, float g, float b, float a = 1.0f);

    GLuint GetDiffuseMap();

    void SetDiffuseMap(GLuint map);

    const glm::vec4& GetDiffuseColor();

    void SetDiffuseColor(const glm::vec4& color);

    void SetDiffuseColor(float r, float g, float b, float a = 1.0f);

    GLuint GetSpecularMap();

    void SetSpecularMap(GLuint map);

    const glm::vec4& GetSpecularColor();

    void SetSpecularColor(const glm::vec4& color);

    void SetSpecularColor(float r, float g, float b, float a = 1.0f);

    GLfloat GetShininess();

    // Usually, artists use value between 0 <= shininess <= 200
    void SetShininess(GLfloat shininess);

    static Material& GetDefaultMaterial();
    
    const std::string& GetName();

    void SetName(const std::string& name);

    const GLuint GetNormalMap()
    {
        return m_normalMap;
    }

    void SetNormalMap(GLuint normalMap)
    {
        m_normalMap = normalMap;
    }


private:
    struct ColorChannel
    {
        ColorChannel()
            : map(0)
            , color(0.0f, 0.0f, 0.0f, 1.0f)
        {
        }

        GLuint map;
        glm::vec4 color;
    };

    std::string m_name;

    ColorChannel m_emissive;
    ColorChannel m_ambient;
    ColorChannel m_diffuse;
    ColorChannel m_specular;
    GLuint m_normalMap;
    GLfloat m_shininess;
};

#endif

/*
    Material.cpp

    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    Material class implementation
*/
#include "Common.h"
#include "Material.h"
#include "Errors.h"

namespace
{
    // Get specific property value and connected texture if any.
    // Value = Property value * Factor property value (if no factor property, multiply by 1).
    FbxDouble3 GetMaterialProperty(const FbxSurfaceMaterial * pMaterial, const char * pPropertyName, const char * pFactorPropertyName, GLuint & pTextureName)
    {
        FbxDouble3 result(0, 0, 0);
        const FbxProperty prop = pMaterial->FindProperty(pPropertyName);
        const FbxProperty factorProp = pMaterial->FindProperty(pFactorPropertyName);
        if (prop.IsValid() && factorProp.IsValid())
        {
            result = prop.Get<FbxDouble3>();
            double factor = factorProp.Get<FbxDouble>();
            if (factor != 1)
            {
                result[0] *= factor;
                result[1] *= factor;
                result[2] *= factor;
            }
        }

        if (prop.IsValid())
        {
            const int TextureCount = prop.GetSrcObjectCount<FbxFileTexture>();
            if (TextureCount)
            {
                const FbxFileTexture* pTexture = prop.GetSrcObject<FbxFileTexture>();
                if (pTexture && pTexture->GetUserDataPtr())
                {
                    pTextureName = *(static_cast<GLuint *>(pTexture->GetUserDataPtr()));
                }
            }
        }

        return result;
    }
}

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

bool Material::Initialize(const FbxSurfaceMaterial * pFbxMaterial)
{
    const FbxDouble3 emissive = GetMaterialProperty(pFbxMaterial, FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor, m_emissive.map);
    m_emissive.color[0] = static_cast<GLfloat>(emissive[0]);
    m_emissive.color[1] = static_cast<GLfloat>(emissive[1]);
    m_emissive.color[2] = static_cast<GLfloat>(emissive[2]);

    const FbxDouble3 ambient = GetMaterialProperty(pFbxMaterial, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor, m_ambient.map);
    m_ambient.color[0] = static_cast<GLfloat>(ambient[0]);
    m_ambient.color[1] = static_cast<GLfloat>(ambient[1]);
    m_ambient.color[2] = static_cast<GLfloat>(ambient[2]);

    const FbxDouble3 diffuse = GetMaterialProperty(pFbxMaterial, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor, m_diffuse.map);
    m_diffuse.color[0] = static_cast<GLfloat>(diffuse[0]);
    m_diffuse.color[1] = static_cast<GLfloat>(diffuse[1]);
    m_diffuse.color[2] = static_cast<GLfloat>(diffuse[2]);

    const FbxDouble3 specular = GetMaterialProperty(pFbxMaterial, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor, m_specular.map);
    m_specular.color[0] = static_cast<GLfloat>(specular[0]);
    m_specular.color[1] = static_cast<GLfloat>(specular[1]);
    m_specular.color[2] = static_cast<GLfloat>(specular[2]);

    FbxProperty shininess = pFbxMaterial->FindProperty(FbxSurfaceMaterial::sShininess);
    if (shininess.IsValid())
    {
        double lShininess = shininess.Get<FbxDouble>();
        m_shininess = static_cast<GLfloat>(lShininess);
    }
    
    return true;
}

Material& Material::GetDefaultMaterial()
{
    static Material s_defaultMaterial;
    s_defaultMaterial.SetAmbientColor(1.0f, .0f, .0f);
    s_defaultMaterial.SetDiffuseColor(1.0f, .0f, .0f);
    s_defaultMaterial.SetSpecularColor(1.0f, .0f, .0f);
    return s_defaultMaterial;
}

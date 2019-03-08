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
        FbxDouble3 lResult(0, 0, 0);
        const FbxProperty lProperty = pMaterial->FindProperty(pPropertyName);
        const FbxProperty lFactorProperty = pMaterial->FindProperty(pFactorPropertyName);
        if (lProperty.IsValid() && lFactorProperty.IsValid())
        {
            lResult = lProperty.Get<FbxDouble3>();
            double lFactor = lFactorProperty.Get<FbxDouble>();
            if (lFactor != 1)
            {
                lResult[0] *= lFactor;
                lResult[1] *= lFactor;
                lResult[2] *= lFactor;
            }
        }

        if (lProperty.IsValid())
        {
            const int lTextureCount = lProperty.GetSrcObjectCount<FbxFileTexture>();
            if (lTextureCount)
            {
                const FbxFileTexture* lTexture = lProperty.GetSrcObject<FbxFileTexture>();
                if (lTexture && lTexture->GetUserDataPtr())
                {
                    pTextureName = *(static_cast<GLuint *>(lTexture->GetUserDataPtr()));
                }
            }
        }

        return lResult;
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
        GET_AND_HANDLE_GL_ERROR();
        m_ambient.map = 0;
    }
}

bool Material::Initialize(const FbxSurfaceMaterial * pMaterial)
{
    const FbxDouble3 lEmissive = GetMaterialProperty(pMaterial, FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor, m_emissive.map);
    m_emissive.color[0] = static_cast<GLfloat>(lEmissive[0]);
    m_emissive.color[1] = static_cast<GLfloat>(lEmissive[1]);
    m_emissive.color[2] = static_cast<GLfloat>(lEmissive[2]);

    const FbxDouble3 lAmbient = GetMaterialProperty(pMaterial, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor, m_ambient.map);
    m_ambient.color[0] = static_cast<GLfloat>(lAmbient[0]);
    m_ambient.color[1] = static_cast<GLfloat>(lAmbient[1]);
    m_ambient.color[2] = static_cast<GLfloat>(lAmbient[2]);

    const FbxDouble3 lDiffuse = GetMaterialProperty(pMaterial, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor, m_diffuse.map);
    m_diffuse.color[0] = static_cast<GLfloat>(lDiffuse[0]);
    m_diffuse.color[1] = static_cast<GLfloat>(lDiffuse[1]);
    m_diffuse.color[2] = static_cast<GLfloat>(lDiffuse[2]);

    const FbxDouble3 lSpecular = GetMaterialProperty(pMaterial, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor, m_specular.map);
    m_specular.color[0] = static_cast<GLfloat>(lSpecular[0]);
    m_specular.color[1] = static_cast<GLfloat>(lSpecular[1]);
    m_specular.color[2] = static_cast<GLfloat>(lSpecular[2]);

    FbxProperty lShininessProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sShininess);
    if (lShininessProperty.IsValid())
    {
        double lShininess = lShininessProperty.Get<FbxDouble>();
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

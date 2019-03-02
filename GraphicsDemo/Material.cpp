#include "Material.h"
#include "Errors.h"
#include <fbxsdk.h>

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

void Material::Free()
{
    if (m_diffuseMap != 0)
    {
        glDeleteTextures(1, &m_diffuseMap);
        GET_AND_HANDLE_GL_ERROR();
    }
}

void Material::Apply()
{
    glBindTexture(GL_TEXTURE_2D, mDiffuse.mTextureName);
    GET_AND_HANDLE_GL_ERROR();
}

bool Material::Initialize(const FbxSurfaceMaterial * pMaterial)
{
    const FbxDouble3 lEmissive = GetMaterialProperty(pMaterial, FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor, mEmissive.mTextureName);
    mEmissive.mColor[0] = static_cast<GLfloat>(lEmissive[0]);
    mEmissive.mColor[1] = static_cast<GLfloat>(lEmissive[1]);
    mEmissive.mColor[2] = static_cast<GLfloat>(lEmissive[2]);

    const FbxDouble3 lAmbient = GetMaterialProperty(pMaterial, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor, mAmbient.mTextureName);
    mAmbient.mColor[0] = static_cast<GLfloat>(lAmbient[0]);
    mAmbient.mColor[1] = static_cast<GLfloat>(lAmbient[1]);
    mAmbient.mColor[2] = static_cast<GLfloat>(lAmbient[2]);

    const FbxDouble3 lDiffuse = GetMaterialProperty(pMaterial, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor, mDiffuse.mTextureName);
    mDiffuse.mColor[0] = static_cast<GLfloat>(lDiffuse[0]);
    mDiffuse.mColor[1] = static_cast<GLfloat>(lDiffuse[1]);
    mDiffuse.mColor[2] = static_cast<GLfloat>(lDiffuse[2]);

    const FbxDouble3 lSpecular = GetMaterialProperty(pMaterial, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor, mSpecular.mTextureName);
    mSpecular.mColor[0] = static_cast<GLfloat>(lSpecular[0]);
    mSpecular.mColor[1] = static_cast<GLfloat>(lSpecular[1]);
    mSpecular.mColor[2] = static_cast<GLfloat>(lSpecular[2]);

    FbxProperty lShininessProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sShininess);
    if (lShininessProperty.IsValid())
    {
        double lShininess = lShininessProperty.Get<FbxDouble>();
        mShinness = static_cast<GLfloat>(lShininess);
    }

    return true;
}

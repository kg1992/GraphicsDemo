#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <glad.h>
#include <glm/glm.hpp>
#include <fbxsdk.h>

class Material
{
public:
    Material();

    void SetProgram(GLuint program);

    GLuint GetProgram();

    void Fill(float* data, int width, int height);

    void Free();

    void Apply();

    // Bake material properties.
    bool Initialize(const FbxSurfaceMaterial * pMaterial);

private:
    GLuint m_program;
    GLuint m_diffuseMap;
    GLuint m_specularMap;

    struct ColorChannel
    {
        ColorChannel() : mTextureName(0)
        {
            mColor[0] = 0.0f;
            mColor[1] = 0.0f;
            mColor[2] = 0.0f;
            mColor[3] = 1.0f;
        }

        GLuint mTextureName;
        GLfloat mColor[4];
    };

    ColorChannel mEmissive;
    ColorChannel mAmbient;
    ColorChannel mDiffuse;
    ColorChannel mSpecular;
    GLfloat mShinness;
};

#endif

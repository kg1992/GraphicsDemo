#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <glad.h>
#include <glm/glm.hpp>

class Material
{
public:
    Material();

    void SetProgram(GLuint program);

    GLuint GetProgram();

    void Fill(float* data, int width, int height);

    void Free();

private:
    GLuint m_program;
    GLuint m_diffuseMap;
    GLuint m_specularMap;
};

#endif


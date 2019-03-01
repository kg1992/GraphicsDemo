#ifndef MESH_H_
#define MESH_H_

#include <glad.h>

class Mesh
{
public:
    void Fill(const GLfloat* vertexData, unsigned long size);

    void Free();

    GLuint GetBo()
    {
        return m_bo;
    }

private:
    GLuint m_bo;
};

#endif MESH_H_

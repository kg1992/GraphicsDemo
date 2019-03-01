#include "Mesh.h"
#include "Errors.h"

void Mesh::Fill(const float * vertexData, unsigned long size)
{
    glGenBuffers(1, &m_bo);
    GET_AND_HANDLE_GL_ERROR();

    glBindBuffer(GL_ARRAY_BUFFER, m_bo);
    GET_AND_HANDLE_GL_ERROR();

    glBufferData(GL_ARRAY_BUFFER, size, vertexData, GL_STATIC_DRAW);
    GET_AND_HANDLE_GL_ERROR();
}

void Mesh::Free()
{
    glDeleteBuffers(1, &m_bo);
    GET_AND_HANDLE_GL_ERROR();
}

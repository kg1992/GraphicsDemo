#include "AttributeArray.h"
#include "Errors.h"

namespace
{
    int GLEnumTypeToSize(GLenum type)
    {
        switch (type)
        {
        case GL_BOOL: return 1;
        case GL_BYTE: return 1;
        case GL_UNSIGNED_BYTE:return 1;
        case GL_SHORT:return 2;
        case GL_UNSIGNED_SHORT:return 2;
        case GL_INT:return 4;
        case GL_UNSIGNED_INT:return 4;
        case GL_FLOAT:return 4;
        case GL_DOUBLE:return 8;
        }
        return 0;
    }
}

AttributeArray::AttributeArray(int size, GLenum type, int stride, int offset)
    : m_bo(0)
    , m_size(size)
    , m_type(type)
    , m_stride(stride)
    , m_offset(offset)
    , m_attributeCount(0)
{
}

void AttributeArray::Fill(unsigned long size, void * data)
{
    if (m_bo == 0)
        glGenBuffers(1, &m_bo);

    glBindBuffer(GL_ARRAY_BUFFER, m_bo);
    GET_AND_HANDLE_GL_ERROR();

    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    GET_AND_HANDLE_GL_ERROR();

    m_attributeCount = m_stride == 0 ? (size / GLEnumTypeToSize(m_type)) : (size / m_stride);
}

void AttributeArray::VertexAttribPointer(int index, GLboolean normalized)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_bo);
    glVertexAttribPointer(index, m_size, m_type, normalized, m_stride, (const void*)m_offset);
    glEnableVertexAttribArray(index);
}

void AttributeArray::Free()
{
    if (m_bo != 0)
    {
        glDeleteBuffers(1, &m_bo);
        m_bo = 0;
    }
}

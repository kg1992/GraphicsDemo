#ifndef ATTRIBUTE_ARRAY_
#define ATTRIBUTE_ARRAY_

#include <glad.h>

class AttributeArray
{
public:
    AttributeArray(int size, GLenum type, int stride, int offset);

    void Fill(unsigned long size, void* data);

    void VertexAttribPointer(int index, GLboolean normalized);

    void Free();

    int GetAttributeCount()
    {
        return m_attributeCount;
    }

private:
    GLuint m_bo;
    int m_size;
    GLenum m_type;
    int m_stride;
    int m_offset;
    int m_attributeCount;
};

#endif

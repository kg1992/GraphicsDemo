/*
    AttributeArray.h

    class AttributeArray definition. AttributeArray encapsulates Vertex Buffer Object(VBO).
*/
#ifndef ATTRIBUTE_ARRAY_
#define ATTRIBUTE_ARRAY_

#include <glad.h>

class AttributeArray
{
public:
    AttributeArray(int size, GLenum type, int stride, const void* offset);

    void Fill(GLsizeiptr size, void* data);

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
    const void* m_offset;
    int m_attributeCount;
};

#endif

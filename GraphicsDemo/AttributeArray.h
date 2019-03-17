/*
    AttributeArray.h

    class AttributeArray definition. AttributeArray encapsulates Vertex Buffer Object(VBO).
*/
#ifndef ATTRIBUTE_ARRAY_
#define ATTRIBUTE_ARRAY_

#include <glad.h>

struct RecordHeader;

class AttributeArray
{
public:
    AttributeArray()
        : m_bo(0)
        , m_size(0)
        , m_type(GL_INVALID_ENUM)
        , m_stride(0)
        , m_offset(0)
        , m_attributeCount(0)
    {
    }

    AttributeArray(int size, GLenum type, int stride, const void* offset);

    void Fill(GLsizeiptr size, void* data);

    void VertexAttribPointer(int index, GLboolean normalized);

    void Free();

    int GetAttributeCount();

    void Serialize(std::ostream& os);

    void Deserialize(std::istream& is);

private:
    GLuint m_bo;
    int m_size;
    GLenum m_type;
    int m_stride;
    const void* m_offset;
    int m_attributeCount;
};

#endif

/*
    AttributeArray.cpp

    class AttributeArray implementation.
*/
#include "Common.h"
#include "AttributeArray.h"
#include "Errors.h"
#include "Serialization.h"

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

AttributeArray::AttributeArray(int size, GLenum type, int stride, const void* offset)
    : m_bo(0)
    , m_size(size)
    , m_type(type)
    , m_stride(stride)
    , m_offset(offset)
    , m_attributeCount(0)
{
}

void AttributeArray::Fill(GLsizeiptr size, void * data)
{
    if (m_bo == 0)
    {
        glGenBuffers(1, &m_bo);
        GET_AND_HANDLE_GL_ERROR();
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_bo);
    GET_AND_HANDLE_GL_ERROR();

    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    GET_AND_HANDLE_GL_ERROR();

    m_attributeCount = static_cast<int>(m_stride == 0 ? (size / GLEnumTypeToSize(m_type)) : (size / m_stride));
}

void AttributeArray::VertexAttribPointer(int index, GLboolean normalized)
{
    
    glBindBuffer(GL_ARRAY_BUFFER, m_bo);
    GET_AND_HANDLE_GL_ERROR();
    if (m_type == GL_INT || m_type == GL_SHORT || m_type == GL_BYTE
      || m_type == GL_UNSIGNED_INT || m_type == GL_UNSIGNED_SHORT || m_type == GL_UNSIGNED_BYTE)
    {
        glVertexAttribIPointer(index, m_size, m_type, m_stride, reinterpret_cast<const void*>(m_offset));
    }
    else
    {
        glVertexAttribPointer(index, m_size, m_type, normalized, m_stride, reinterpret_cast<const void*>(m_offset));
    }
    GET_AND_HANDLE_GL_ERROR();
    glEnableVertexAttribArray(index);
    GET_AND_HANDLE_GL_ERROR();
}

void AttributeArray::Free()
{
    if (m_bo != 0)
    {
        glDeleteBuffers(1, &m_bo);
        GET_AND_HANDLE_GL_ERROR();
        m_bo = 0;
    }
}

int AttributeArray::GetAttributeCount()
{
    return m_attributeCount;
}

namespace
{
    void SerializeBufferObject(std::ostream& os, GLuint bo)
    {
        glBindBuffer(GL_ARRAY_BUFFER, bo);
        GET_AND_HANDLE_GL_ERROR();
        
        GLint size = 0;
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
        GET_AND_HANDLE_GL_ERROR();
        assert(size != 0);
        
        // !! assume that size is size in bytes
        std::vector<char> buffer(size);
        glGetBufferSubData(GL_ARRAY_BUFFER, 0, size, buffer.data());
        GET_AND_HANDLE_GL_ERROR();

        Serialization::Write(os, size);
        os.write(buffer.data(), size);
    }

    void DeserializeBufferObject(std::istream& is, GLuint& bo)
    {
        int bufferSize;
        Serialization::Read(is, bufferSize);
        std::vector<char> buffer(bufferSize, '\0');
        is.read(buffer.data(), buffer.size());

        glGenBuffers(1, &bo);
        GET_AND_HANDLE_GL_ERROR();

        glBindBuffer(GL_ARRAY_BUFFER, bo);
        GET_AND_HANDLE_GL_ERROR();

        glBufferData(GL_ARRAY_BUFFER, buffer.size(), buffer.data(), GL_STATIC_DRAW);
        GET_AND_HANDLE_GL_ERROR();
    }
}

void AttributeArray::Serialize(std::ostream & os)
{
    Serialization::Write(os, m_size);
    Serialization::Write(os, m_type);
    Serialization::Write(os, m_stride);
    Serialization::Write(os, m_offset);
    Serialization::Write(os, m_attributeCount);

    SerializeBufferObject(os, m_bo);
}

void AttributeArray::Deserialize(std::istream & is)
{
    Serialization::Read(is, m_size);
    Serialization::Read(is, m_type);
    Serialization::Read(is, m_stride);
    Serialization::Read(is, m_offset);
    Serialization::Read(is, m_attributeCount);

    DeserializeBufferObject(is, m_bo);
}

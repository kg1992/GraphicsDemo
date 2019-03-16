/*
    Mesh.cpp

    class Mesh implementation.
*/
#include "Common.h"
#include "Mesh.h"
#include "Errors.h"
#include "Serialization.h"

void Mesh::Free()
{
    m_subMeshes.clear();
    while (!m_attributeArrays.empty())
    {
        m_attributeArrays.back().Free();
        m_attributeArrays.pop_back();
    }
}

bool Mesh::HasSubMesh()
{
    return !m_subMeshes.empty();
}

void Mesh::AddSubMesh(const SubMesh & subMesh)
{
    m_subMeshes.push_back(subMesh);
}

int Mesh::GetSubMeshCount()
{
    return static_cast<int>(m_subMeshes.size());
}

Mesh::SubMesh Mesh::GetSubMesh(int i)
{
    return m_subMeshes[i];
}

void Mesh::AddAttributeArray(AttributeArray & aa)
{
    m_attributeArrays.push_back(aa);
}

AttributeArray& Mesh::GetAttributeArray(int index)
{
    return m_attributeArrays[index];
}

void Mesh::Apply()
{
    for (int i = 0; i < m_attributeArrays.size(); ++i)
    {
        m_attributeArrays[i].VertexAttribPointer(i, GL_FALSE);
    }
}

void Mesh::Serialize(std::ostream& os)
{
    int subMeshCount = static_cast<int>(m_subMeshes.size());
    Serialization::Write(os, subMeshCount);
    for (int i = 0; i < m_subMeshes.size(); ++i)
    {
        SubMesh& subMesh = m_subMeshes[i];
        Serialization::Write(os, subMesh);
    }

    int attributeArrayCount = static_cast<int>(m_attributeArrays.size());
    Serialization::Write(os, attributeArrayCount);
    for (int i = 0; i < m_attributeArrays.size(); ++i)
    {
        AttributeArray& attrArray = m_attributeArrays[i];
        attrArray.Serialize(os);
    }
}

void Mesh::Deserialize(std::istream& is)
{
    int subMeshCount = 0;
    Serialization::Read(is, subMeshCount);
    for (int i = 0; i < subMeshCount; ++i)
    {
        SubMesh subMesh;
        Serialization::Read(is, subMesh);
        m_subMeshes.push_back(subMesh);
    }

    int attributeArrayCount = 0;
    Serialization::Read(is, attributeArrayCount);
    for (int i = 0; i < attributeArrayCount; ++i)
    {
        AttributeArray attrArray;
        attrArray.Deserialize(is);
        m_attributeArrays.push_back(attrArray);
    }
}

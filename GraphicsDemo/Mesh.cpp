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
    if (m_position)
    {
        m_position->Free();
        m_position.reset();
    }
    if (m_uv)
    {
        m_uv->Free();
        m_uv.reset();
    }
    if (m_normal)
    {
        m_normal->Free();
        m_normal.reset();
    }
    if (m_bones)
    {
        m_bones->Free();
        m_bones.reset();
    }
    if (m_weights)
    {
        m_weights->Free();
        m_weights.reset();
    }
    if (m_tangent)
    {
        m_tangent->Free();
        m_tangent.reset();
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

void Mesh::Apply()
{
    if (m_position)
    {
        m_position->VertexAttribPointer(0, GL_FALSE);
    }
    if (m_uv)
    {
        m_uv->VertexAttribPointer(1, GL_FALSE);
    }
    if (m_normal)
    {
        m_normal->VertexAttribPointer(2, GL_FALSE);
    }
    if (m_bones)
    {
        m_bones->VertexAttribPointer(3, GL_FALSE);
    }
    if (m_weights)
    {
        m_weights->VertexAttribPointer(4, GL_FALSE);
    }
    if (m_tangent)
    {
        m_tangent->VertexAttribPointer(5, GL_FALSE);
    }
}

namespace
{
    void SerializeAttributeArrayPointer(std::ostream& os, std::shared_ptr<AttributeArray> pAttributeArray)
    {
        int count = pAttributeArray ? 1 : 0;
        Serialization::Write(os, count);
        if (count)
        {
            pAttributeArray->Serialize(os);
        }
    }

    void DeserializeAttributeArrayPointer(std::istream& is, std::shared_ptr<AttributeArray>& pAttributeArray)
    {
        int count;
        Serialization::Read(is, count);
        if (count)
        {
            pAttributeArray.reset(new AttributeArray());
            pAttributeArray->Deserialize(is);
        }
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

    SerializeAttributeArrayPointer(os, m_position);
    SerializeAttributeArrayPointer(os, m_uv);
    SerializeAttributeArrayPointer(os, m_normal);
    SerializeAttributeArrayPointer(os, m_bones);
    SerializeAttributeArrayPointer(os, m_weights);
    SerializeAttributeArrayPointer(os, m_tangent);

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

    DeserializeAttributeArrayPointer(is, m_position);
    DeserializeAttributeArrayPointer(is, m_uv);
    DeserializeAttributeArrayPointer(is, m_normal);
    DeserializeAttributeArrayPointer(is, m_bones);
    DeserializeAttributeArrayPointer(is, m_weights);
    DeserializeAttributeArrayPointer(is, m_tangent);
}

void Mesh::SetDefaultSubMesh()
{
    m_subMeshes.push_back(SubMesh
    {
            0, m_position->GetAttributeCount()
    });
}

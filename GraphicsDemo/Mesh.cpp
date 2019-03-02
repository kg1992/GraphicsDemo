#include "Mesh.h"
#include "Errors.h"

void Mesh::Free()
{
    m_subMeshes.clear();
    while (!m_attributeArrays.empty())
    {
        m_attributeArrays.back().Free();
        m_attributeArrays.pop_back();
    }
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

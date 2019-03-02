#ifndef MESH_H_
#define MESH_H_

#include <glad.h>
#include <fbxsdk.h>
#include <vector>
#include "AttributeArray.h"

class Mesh
{
public:
    void Free();

    void AddAttributeArray(AttributeArray& aa);

    AttributeArray& GetAttributeArray(int index);

    void Apply();

    struct SubMesh
    {
        int begin;
        int vertCount;
    };

    bool HasSubMesh()
    {
        return !m_subMeshes.empty();
    }

    void AddSubMesh(const SubMesh& subMesh)
    {
        m_subMeshes.push_back(subMesh);
    }

    int GetSubMeshCount()
    {
        return m_subMeshes.size();
    }

    SubMesh GetSubMesh(int i)
    {
        return m_subMeshes[i];
    }

private:
    std::vector<SubMesh> m_subMeshes;
    std::vector<AttributeArray> m_attributeArrays;
};

#endif MESH_H_

/*
    Mesh.h

    class Mesh definition. class Mesh consists of multiple AttributeArray and serves as rendering cache used for OpenGL.

    Todo:

        Use SubMesh even when there is only one section.

        Change implementation to use VAO.

    Changes:
*/
#ifndef MESH_H_
#define MESH_H_

#include "AttributeArray.h"

struct RecordHeader;

class Mesh
{
public:
    struct SubMesh
    {
        int begin;
        int vertCount;
    };

    void Apply();

    void Free();

    bool HasSubMesh();

    void AddSubMesh(const SubMesh& subMesh);

    int GetSubMeshCount();

    SubMesh GetSubMesh(int i);

    void Serialize(std::ostream& os);

    void Deserialize(std::istream& is);

    std::shared_ptr<AttributeArray> GetPositionAttributeArray()
    {
        return m_position;
    }

    void SetPositionAttributeArray(std::shared_ptr<AttributeArray> aa)
    {
        m_position = aa;
    }

    std::shared_ptr<AttributeArray> GetUvAttributeArray()
    {
        return m_uv;
    }

    void SetUvAttributeArray(std::shared_ptr<AttributeArray> aa)
    {
        m_uv = aa;
    }

    std::shared_ptr<AttributeArray> GetNormalAttributeArray()
    {
        return m_normal;
    }

    void SetNormalAttributeArray(std::shared_ptr<AttributeArray> aa)
    {
        m_normal = aa;
    }

    std::shared_ptr<AttributeArray> GetBonesAttributeArray()
    {
        return m_bones;
    }

    void SetBonesAttributeArray(std::shared_ptr<AttributeArray> aa)
    {
        m_bones = aa;
    }

    std::shared_ptr<AttributeArray> GetWeightsAttributeArray()
    {
        return m_weights;
    }

    void SetWeightsAttributeArray(std::shared_ptr<AttributeArray> aa)
    {
        m_weights = aa;
    }

    std::shared_ptr<AttributeArray> GetTangentAttributeArray()
    {
        return m_tangent;
    }

    void SetTangentAttributeArray(std::shared_ptr<AttributeArray> aa)
    {
        m_tangent = aa;
    }

    void SetDefaultSubMesh();

private:
    std::vector<SubMesh> m_subMeshes;
    std::shared_ptr<AttributeArray> m_position;
    std::shared_ptr<AttributeArray> m_uv;
    std::shared_ptr<AttributeArray> m_normal;
    std::shared_ptr<AttributeArray> m_bones;
    std::shared_ptr<AttributeArray> m_weights;
    std::shared_ptr<AttributeArray> m_tangent;
};

#endif MESH_H_

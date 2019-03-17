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

    void AddAttributeArray(AttributeArray& aa);

    AttributeArray& GetAttributeArray(int index);

    void Apply();

    void Free();

    bool HasSubMesh();

    void AddSubMesh(const SubMesh& subMesh);

    int GetSubMeshCount();

    SubMesh GetSubMesh(int i);

    void Serialize(std::ostream& os);

    void Deserialize(std::istream& is);

private:
    std::vector<SubMesh> m_subMeshes;
    std::vector<AttributeArray> m_attributeArrays;
};

#endif MESH_H_

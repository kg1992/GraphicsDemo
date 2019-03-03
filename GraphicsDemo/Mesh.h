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

#include <glad.h>
#include <fbxsdk.h>
#include <vector>
#include "AttributeArray.h"

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

private:
    std::vector<SubMesh> m_subMeshes;
    std::vector<AttributeArray> m_attributeArrays;
};

#endif MESH_H_

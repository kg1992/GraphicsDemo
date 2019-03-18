#include "Common.h"
#include "Mesh.h"

std::shared_ptr<Mesh> GenerateCube()
{
    static const float vertex_positions[] =
    {
        -0.25f,  0.25f, -0.25f,
        -0.25f, -0.25f, -0.25f,
         0.25f, -0.25f, -0.25f,

         0.25f, -0.25f, -0.25f,
         0.25f,  0.25f, -0.25f,
        -0.25f,  0.25f, -0.25f,

         0.25f, -0.25f, -0.25f,
         0.25f, -0.25f,  0.25f,
         0.25f,  0.25f, -0.25f,

         0.25f, -0.25f,  0.25f,
         0.25f,  0.25f,  0.25f,
         0.25f,  0.25f, -0.25f,

         0.25f, -0.25f,  0.25f,
        -0.25f, -0.25f,  0.25f,
         0.25f,  0.25f,  0.25f,

        -0.25f, -0.25f,  0.25f,
        -0.25f,  0.25f,  0.25f,
         0.25f,  0.25f,  0.25f,

        -0.25f, -0.25f,  0.25f,
        -0.25f, -0.25f, -0.25f,
        -0.25f,  0.25f,  0.25f,

        -0.25f, -0.25f, -0.25f,
        -0.25f,  0.25f, -0.25f,
        -0.25f,  0.25f,  0.25f,

        -0.25f, -0.25f,  0.25f,
         0.25f, -0.25f,  0.25f,
         0.25f, -0.25f, -0.25f,

         0.25f, -0.25f, -0.25f,
        -0.25f, -0.25f, -0.25f,
        -0.25f, -0.25f,  0.25f,

        -0.25f,  0.25f, -0.25f,
         0.25f,  0.25f, -0.25f,
         0.25f,  0.25f,  0.25f,

         0.25f,  0.25f,  0.25f,
        -0.25f,  0.25f,  0.25f,
        -0.25f,  0.25f, -0.25f
    };

    std::shared_ptr<Mesh> pMesh(new Mesh());
    std::shared_ptr<AttributeArray> aaPositions(new AttributeArray(3, GL_FLOAT, 0, 0));
    aaPositions->Fill(sizeof(vertex_positions), (void*)vertex_positions);
    pMesh->SetPositionAttributeArray(aaPositions);

    pMesh->SetDefaultSubMesh();

    return pMesh;
}


std::shared_ptr<Mesh> GeneratePlane()
{
    float positions[] = 
    {
        -1.0f, 0.0f, -1.0f,
        -1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,

        -1.0f, 0.0f, -1.0f,
        1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, -1.0f,
    };

    std::shared_ptr<Mesh> pMesh(new Mesh());
    std::shared_ptr<AttributeArray> aaPosition(new AttributeArray(3, GL_FLOAT, 0, 0));
    aaPosition->Fill(sizeof(positions), positions);
    pMesh->SetPositionAttributeArray(aaPosition);

    float uvs[] = 
    {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,

        0.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    std::shared_ptr<AttributeArray> aaUv(new AttributeArray(2, GL_FLOAT, 0, 0));
    aaUv->Fill(sizeof(uvs), uvs);
    pMesh->SetUvAttributeArray(aaUv);

    float normals[] =
    {
        0, 1.0f, 0,
        0, 1.0f, 0,
        0, 1.0f, 0,

        0, 1.0f, 0,
        0, 1.0f, 0,
        0, 1.0f, 0,
    };

    std::shared_ptr<AttributeArray> aaNormal(new AttributeArray(3, GL_FLOAT, 0, 0));
    aaNormal->Fill(sizeof(normals), normals);
    pMesh->SetNormalAttributeArray(aaNormal);

    pMesh->SetDefaultSubMesh();

    return pMesh;
}
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
    AttributeArray aaPositions(3, GL_FLOAT, 0, 0);
    aaPositions.Fill(sizeof(vertex_positions), (void*)vertex_positions);
    pMesh->AddAttributeArray(aaPositions);
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
    AttributeArray aaPosition(3, GL_FLOAT, 0, 0);
    aaPosition.Fill(sizeof(positions), positions);
    pMesh->AddAttributeArray(aaPosition);

    float uvs[] = 
    {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,

        0.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };
    AttributeArray aaUv(2, GL_FLOAT, 0, 0);
    aaUv.Fill(sizeof(uvs), uvs);
    pMesh->AddAttributeArray(aaUv);

    float normals[] =
    {
        0, 1.0f, 0,
        0, 1.0f, 0,
        0, 1.0f, 0,

        0, 1.0f, 0,
        0, 1.0f, 0,
        0, 1.0f, 0,
    };

    AttributeArray aaNormal(3, GL_FLOAT, 0, 0);
    aaNormal.Fill(sizeof(normals), normals);
    pMesh->AddAttributeArray(aaNormal);

    return pMesh;
}
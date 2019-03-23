#include "Common.h"
#include "Geometry.h"
#include "Mesh.h"
#include "Errors.h"

namespace
{
    const float PI = glm::pi<float>();
}

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

GLuint GeometryToVBO(const Geometry& geometry)
{
    GLuint buffer;
    glGenBuffers(1, &buffer);
    GET_AND_HANDLE_GL_ERROR();

    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    GET_AND_HANDLE_GL_ERROR();

    glBufferData(GL_ARRAY_BUFFER
        , geometry.GetVertexCount() * sizeof(Geometry::Point)
        , geometry.GetVertexBufferPointer(), GL_STATIC_DRAW);
    GET_AND_HANDLE_GL_ERROR();    

    return buffer;
}

Frustum::Frustum(float nw, float nh, float fw, float fh, float nearz, float farz)
    : m_vertices()
    , m_faces()
    , m_edges()
{
    assert(nw > 0 && nh > 0 && fw > nw && fh > nh && nearz > 0 && farz > nearz);
    float hnw = nw * 0.5f, hnh = nh * 0.5f, hfw = fw * 0.5f, hfh = fh * 0.5f;
    m_vertices[0] = Point(-hnw, -hnh, -nearz);
    m_vertices[1] = Point( hnw, -hnh, -nearz);
    m_vertices[2] = Point( hnw,  hnh, -nearz);
    m_vertices[3] = Point(-hnw,  hnh, -nearz);
    m_vertices[4] = Point(-hfw, -hfh, -farz);
    m_vertices[5] = Point( hfw, -hfh, -farz);
    m_vertices[6] = Point( hfw,  hfh, -farz);
    m_vertices[7] = Point(-hfw,  hfh, -farz);

    m_faces[0] = Face(0, 1, 2);
    m_faces[1] = Face(0, 2, 3); // +Z
    m_faces[2] = Face(1, 5, 6);
    m_faces[3] = Face(1, 6, 2); // +X
    m_faces[4] = Face(2, 6, 7);
    m_faces[5] = Face(2, 7, 3); // +Y
    m_faces[6] = Face(3, 7, 4);
    m_faces[7] = Face(3, 4, 0); // -X
    m_faces[8] = Face(0, 4, 5);
    m_faces[9] = Face(0, 5, 1); // -Y
    m_faces[10] = Face(4, 7, 6);
    m_faces[11] = Face(4, 6, 5); // -Z

    m_edges[0] = Edge(0, 1);
    m_edges[1] = Edge(1, 2);
    m_edges[2] = Edge(2, 3);
    m_edges[3] = Edge(3, 0);
    m_edges[4] = Edge(0, 4); 
    m_edges[5] = Edge(2, 6);
    m_edges[6] = Edge(3, 7);
    m_edges[7] = Edge(1, 5);
    m_edges[8] = Edge(4, 5);
    m_edges[9] = Edge(5, 6);
    m_edges[10] = Edge(6, 7);
    m_edges[11] = Edge(7, 4);
}

Frustum::Frustum(float fov, float aspectRatio, float nearz, float farz)
    : Frustum(std::tan(fov*0.5f) * nearz * aspectRatio
            , std::tan(fov*0.5f) * nearz
            , std::tan(fov*0.5f) * farz * aspectRatio
            , std::tan(fov*0.5f) * farz
            , nearz
            , farz)
{
}

float Frustum::GetConvergingZ()
{
    float nw = m_vertices[0].x
    , fw = m_vertices[4].x
    , n = -m_vertices[0].z
    , f = -m_vertices[4].z;

    return (fw * n - nw * f) / (fw - nw);
}

// The constructor will make a UV sphere, whose top is heading +Y.
// To avoid degnerate case, user are to provide rings >= 2, sectors >=3 or the user will get the assertion failure.
// In the end, vertex count will be (rings + 1) * (sectors + 1).
// Note : The code will generate duplicate top and bottom vertices as many as sectors.
//        The code will also generate duplicate side vertex for each ring. (n sectors -> n + 1 vertices on each ring edge)
//        Those duplicates allow hard edges to be implemented at the top and bottom area.
//        They also help uv mapping onto the rectangular plane.
// Refernece : http://www.songho.ca/opengl/gl_sphere.html
UVSphere::UVSphere(float radius, int rings, int sectors)
    : m_vertices()
    , m_faces()
    , m_edges()
{
    assert(radius > 0 && rings >= 2 && sectors >= 3);
    float const R = 1.f / rings;
    float const S = 1.f / sectors;
    m_vertices.reserve((rings + 1) * (sectors + 1));
    for (int r = 0; r <= rings; ++r)
    {
        const float Phi = PI * 0.5f - r * R;
        const float CosPhi = std::cos(Phi);
        const float SinPhi = std::sin(Phi);
        for (int s = 0; s <= sectors; ++s)
        {
            const float Theta = (s != sectors - 1) ? (s * S) : 0.f;
            const float CosTheta = std::cos(Theta);
            const float SinTheta = std::sin(Theta);
            const float X = SinTheta * SinPhi * radius;
            const float Y = CosPhi * radius;
            const float Z = CosTheta * CosPhi * radius;
            m_vertices.emplace_back(X, Y, Z);
        }
    }

    m_faces.reserve(rings * sectors * 2);
    const int VertexPerRing = sectors + 1;
    for (int r = 0; r < rings; ++r)
    {
        for (int s = 0; s < sectors; ++s)
        {
            const int A = s + r * VertexPerRing;
            const int B = A + VertexPerRing;
            const int C = B + 1;
            const int D = A + 1;
            m_faces.emplace_back(A, B, C);
            m_faces.emplace_back(A, C, D);
        }
    }

    m_edges.reserve(sectors * (2 * rings - 1));
    for (int r = 0; r < rings; ++r)
    {
        for (int s = 0; s < sectors; ++s)
        {
            const int A = s + r * VertexPerRing;
            const int B = A + VertexPerRing;
            const int D = A + 1;
            if (r != 0)
            {
                m_edges.emplace_back(A, B);
            }
            m_edges.emplace_back(A, D);
        }
    }
}

#ifndef GEOMETRY_H_
#define GEOMETRY_H_

class Mesh;

std::shared_ptr<Mesh> GenerateCube();
std::shared_ptr<Mesh> GeneratePlane();

class Geometry
{
public:
    typedef glm::vec3 Point;
    typedef glm::ivec3 Face;
    typedef glm::ivec2 Edge;

    virtual int GetVertexCount() const = 0;
    virtual Point GetVertex(int i) const = 0;
    virtual const Point* GetVertexBufferPointer() const = 0;
    virtual Point* GetVertexBufferPointer() = 0;

    virtual int GetFaceCount() const = 0;
    virtual Face GetFace(int i) const = 0;

    virtual int GetEdgeCount() const = 0;
    virtual Edge GetEdge(int i) const = 0;
};

// Frustum Geometry
//  Always directed toward -z direction; top plane is at z = -nearz; bottom plane is at z = -farz;
class Frustum : public Geometry
{
public:
    enum {
        VERTEX_COUNT = 8,
        FACE_COUNT = 12,
        EDGE_COUNT = 12,
    };

    Frustum(float nw, float nh, float fw, float fh, float nearz, float farz);
    Frustum(float fov, float aspectRatio, float nearz, float farz);

    int GetVertexCount() const override { return VERTEX_COUNT;}
    Point GetVertex(int i) const override
    {
        return m_vertices[i];
    }
    virtual const Point* GetVertexBufferPointer() const override { return m_vertices; }
    virtual Point* GetVertexBufferPointer() override { return m_vertices; }

    int GetFaceCount() const override { return FACE_COUNT; }
    Face GetFace(int i) const override
    {
        return m_faces[i];
    }

    int GetEdgeCount() const override { return EDGE_COUNT; }
    Edge GetEdge(int i) const override
    {
        return m_edges[i];
    }

    float GetConvergingZ();

private:
    Point m_vertices[VERTEX_COUNT];
    Face m_faces[FACE_COUNT];
    Edge m_edges[EDGE_COUNT];
};

class UVSphere : public Geometry
{
public:
    UVSphere(float radius, int rings, int sectors);

    int GetVertexCount() const override { return static_cast<int>(m_vertices.size()); }
    Point GetVertex(int i) const override
    {
        return m_vertices[i];
    }
    virtual const Point* GetVertexBufferPointer() const override { return m_vertices.data(); }
    virtual Point* GetVertexBufferPointer() override { return m_vertices.data(); }

    int GetFaceCount() const override { return static_cast<int>(m_faces.size()); }
    Face GetFace(int i) const override
    {
        return m_faces[i];
    }

    int GetEdgeCount() const override { return static_cast<int>(m_edges.size()); }
    Edge GetEdge(int i)
    {
        return m_edges[i];
    }

private:
    std::vector<Point> m_vertices;
    std::vector<Face> m_faces;
    std::vector<Edge> m_edges;
};

#endif

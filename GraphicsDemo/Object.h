#ifndef OBJECT_H_
#define OBJECT_H_

#include <vector>
#include <glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Mesh;
class Material;

class Object
{
public:
    Object();

    void SetPosition(const glm::vec3& pos);

    const glm::vec3& GetPosition();
    
    void SetScale(const glm::vec3& scl);

    const glm::vec3& GetScale();

    void SetRotation(const glm::quat& rot);

    const glm::quat& GetRotation();
    
    void SetTransformMatrix(const glm::mat4x4& transformMatrix);

    void ApplyTransformMatrix(const glm::mat4x4& transformMatrix);

    void UpdateTransformMatrix();

    const glm::mat4x4& GetTransformMatrix();

    void AddMaterial(std::shared_ptr<Material> ptr);

    std::shared_ptr<Mesh> GetMesh()
    {
        return m_mesh;
    }

    void SetMesh(std::shared_ptr<Mesh> mesh)
    {
        m_mesh = mesh;
    }

    std::shared_ptr<Material> GetMaterial()
    {
        return m_material;
    }

    std::shared_ptr<Material> GetMaterial(int index)
    {
        return m_materials[index];
    }

    void SetMaterial(std::shared_ptr<Material> material)
    {
        m_material = material;
    }

    void Free();

    void Render();


private:
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::quat m_rotation;
    glm::mat4x4 m_mvMatrix;

    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<Material> m_material;
    std::vector<std::shared_ptr<Material>> m_materials;
};

#endif

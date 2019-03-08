/*
    Object.h

    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    Object class definition.
*/
#ifndef OBJECT_H_
#define OBJECT_H_

class Mesh;
class Material;
class ShaderProgram;

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

    std::shared_ptr<Material> GetMaterial(int index)
    {
        return m_materials[index];
    }

    void Free();

    void Render(ShaderProgram& program);

private:
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::quat m_rotation;
    glm::mat4x4 m_mvMatrix;

    std::shared_ptr<Mesh> m_mesh;
    std::vector<std::shared_ptr<Material>> m_materials;
};

#endif

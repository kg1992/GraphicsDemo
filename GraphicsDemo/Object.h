/*
    Object.h

    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    Object class definition.
*/
#ifndef OBJECT_H_
#define OBJECT_H_

#include "Skeleton.h"
#include "Animator.h"

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

    void Update();

    const glm::mat4x4& GetTransformMatrix();

    void AddMaterial(std::shared_ptr<Material> ptr);

    std::shared_ptr<Material> GetMaterial(int index)
    {
        return m_materials[index];
    }

    void AddMesh(std::shared_ptr<Mesh> mesh)
    {
        m_meshes.push_back(mesh);
    }

    std::shared_ptr<Mesh> GetMesh(int index)
    {
        return m_meshes[index];
    }

    void Free();

    void Render(ShaderProgram& program);

    std::shared_ptr<Skeleton> GetSkeleton();

    void SetSkeleton(std::shared_ptr<Skeleton> pSkeleton);    
    
    void SetAnimator(std::shared_ptr<Animator> pAnimator);

private:
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::quat m_rotation;
    glm::mat4x4 m_mvMatrix;

    std::vector<std::shared_ptr<Mesh>> m_meshes;
    std::vector<std::shared_ptr<Material>> m_materials;
    std::shared_ptr<Skeleton> m_pSkeleton;
    std::shared_ptr<Animator> m_pAnimator;
};

#endif

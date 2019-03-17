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
struct RecordHeader;

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

    //
    // Must be called every frame.
    // Updates animator.
    //
    void Update();

    // Gets the model to world transform matrix
    const glm::mat4x4& GetTransformMatrix();

    // Adds a material.
    void AddMaterial(std::shared_ptr<Material> ptr);

    // Gets 'index'th material
    std::shared_ptr<Material> GetMaterial(int index);

    // Adds a mesh to be rendered.
    void AddMesh(std::shared_ptr<Mesh> mesh);

    // Gets 'index'th mesh
    std::shared_ptr<Mesh> GetMesh(int index);

    // Free object items
    void Free();

    // Renders object with given program.
    void Render(ShaderProgram& program);

    // Gets skeleton associated with this object.
    std::shared_ptr<Skeleton> GetSkeleton();

    // Sets skeleton. the last set skeleton will be released.
    void SetSkeleton(std::shared_ptr<Skeleton> pSkeleton);    
    
    // Sets animator. the last set animator will be released.
    void SetAnimator(std::shared_ptr<Animator> pAnimator);

    // Serialize object to the given stream 'os' in binary format
    void Serialize(std::ostream& os);

    // Deserialize objet to the given stream 'is' in binary format
    void Deserialize(std::istream& is);

private:
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::quat m_rotation;
    // model to world transform matrix
    glm::mat4x4 m_mwMatrix;

    std::vector<std::shared_ptr<Mesh>> m_meshes;
    std::vector<std::shared_ptr<Material>> m_materials;
    std::shared_ptr<Skeleton> m_pSkeleton;
    std::shared_ptr<Animator> m_pAnimator;

    //
    // Updates Model-World Transform Matrix when position, scale, rotation has been changed.
    //
    void UpdateTransformMatrix();
};

#endif

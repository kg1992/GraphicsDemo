#ifndef MESH_H_
#define MESH_H_

#include <glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Mesh
{
public:
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

private:
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::quat m_rotation;
    glm::mat4x4 m_mvMatrix;
};

#endif

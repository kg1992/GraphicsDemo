#include "Mesh.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Errors.h"

void Mesh::SetPosition(const glm::vec3& pos)
{
    m_position = pos;
    UpdateTransformMatrix();
}

const glm::vec3& Mesh::GetPosition()
{
    return m_position;
}

void Mesh::SetScale(const glm::vec3& scl)
{
    m_scale = scl;
    UpdateTransformMatrix();
}

const glm::vec3& Mesh::GetScale()
{
    return m_scale;
}

void Mesh::SetRotation(const glm::quat& rot)
{
    m_rotation = rot;
    UpdateTransformMatrix();
}

const glm::quat& Mesh::GetRotation()
{
    return m_rotation;
}

// ref : https://stackoverflow.com/questions/17918033/glm-decompose-mat4-into-translation-and-rotation
void Mesh::SetTransformMatrix(const glm::mat4x4& transformMatrix)
{
    glm::vec3 skew;
    glm::vec4 proj;
    glm::decompose(transformMatrix, m_position, m_rotation, m_scale, skew, proj);
    m_mvMatrix = transformMatrix;
}

void Mesh::ApplyTransformMatrix(const glm::mat4x4& transformMatrix)
{
    SetTransformMatrix(transformMatrix * m_mvMatrix);
}

void Mesh::UpdateTransformMatrix()
{
    m_mvMatrix = glm::identity<glm::mat4x4>();
    m_mvMatrix = glm::scale(m_mvMatrix, m_scale);
    m_mvMatrix *= glm::toMat4(m_rotation);
    m_mvMatrix = glm::translate(m_mvMatrix, m_position);
}

const glm::mat4x4& Mesh::GetTransformMatrix()
{
    return m_mvMatrix;
}
#include "Object.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Errors.h"
#include "Mesh.h"
#include "Material.h"

Object::Object()
    : m_position()
    , m_scale(1,1,1)
    , m_rotation(glm::identity<glm::quat>())
    , m_mvMatrix(glm::identity<glm::mat4x4>())
    , m_mesh()
{
}

void Object::SetPosition(const glm::vec3& pos)
{
    m_position = pos;
    UpdateTransformMatrix();
}

const glm::vec3& Object::GetPosition()
{
    return m_position;
}

void Object::SetScale(const glm::vec3& scl)
{
    m_scale = scl;
    UpdateTransformMatrix();
}

const glm::vec3& Object::GetScale()
{
    return m_scale;
}

void Object::SetRotation(const glm::quat& rot)
{
    m_rotation = rot;
    UpdateTransformMatrix();
}

const glm::quat& Object::GetRotation()
{
    return m_rotation;
}

// ref : https://stackoverflow.com/questions/17918033/glm-decompose-mat4-into-translation-and-rotation
void Object::SetTransformMatrix(const glm::mat4x4& transformMatrix)
{
    glm::vec3 skew;
    glm::vec4 proj;
    glm::decompose(transformMatrix, m_position, m_rotation, m_scale, skew, proj);
    m_mvMatrix = transformMatrix;
}

void Object::ApplyTransformMatrix(const glm::mat4x4& transformMatrix)
{
    SetTransformMatrix(transformMatrix * m_mvMatrix);
}

void Object::UpdateTransformMatrix()
{
    m_mvMatrix = glm::identity<glm::mat4x4>();
    m_mvMatrix = glm::scale(m_mvMatrix, m_scale);
    m_mvMatrix *= glm::toMat4(m_rotation);
    m_mvMatrix = glm::translate(m_mvMatrix, m_position);
}

const glm::mat4x4& Object::GetTransformMatrix()
{
    return m_mvMatrix;
}

void Object::AddMaterial(std::shared_ptr<Material> pMaterial)
{
    m_materials.push_back(pMaterial);
}

void Object::Free()
{
    while (!m_materials.empty())
    {
        m_materials.back()->Free();
        m_materials.pop_back();
    }

    if (m_mesh)
    {
        m_mesh->Free();
        m_mesh.reset();
    }
}

void Object::Render()
{
    m_mesh->Apply();

    if (m_mesh->HasSubMesh())
    {
        for (int i = 0; i < m_mesh->GetSubMeshCount(); ++i)
        {
            m_materials[i]->Apply();
            Mesh::SubMesh subMesh = m_mesh->GetSubMesh(i);
            glDrawArrays(GL_TRIANGLES, subMesh.begin, subMesh.vertCount);
        }
    }
    else {
        glDrawArrays(GL_TRIANGLES, 0, m_mesh->GetAttributeArray(0).GetAttributeCount());
    }
}

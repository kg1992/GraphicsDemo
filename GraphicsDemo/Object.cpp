/*
    Object.cpp

    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    Object class implementation

    Todo:
        Use VAO
*/
#include "Common.h"
#include "Object.h"
#include "Errors.h"
#include "Mesh.h"
#include "Material.h"
#include "ShaderProgram.h"

namespace
{
    void SendMaterial(ShaderProgram& program, Material& material)
    {
        if (material.GetAmbientMap() != 0)
            program.TrySendUniform("material.ambientMap", (int)material.GetAmbientMap());
        program.TrySendUniform("material.ka", material.GetAmbientColor()[0], material.GetAmbientColor()[1], material.GetAmbientColor()[2]);
        if (material.GetDiffuseMap() != 0)
            program.TrySendUniform("material.diffuseMap", (int)material.GetDiffuseMap());
        program.TrySendUniform("material.kd", material.GetDiffuseColor()[0], material.GetDiffuseColor()[1], material.GetDiffuseColor()[2]);
        if (material.GetSpecularMap() != 0)
            program.TrySendUniform("material.specularMap", (int)material.GetSpecularMap());
        program.TrySendUniform("material.ks", material.GetSpecularColor()[0], material.GetSpecularColor()[1], material.GetSpecularColor()[2]);
        program.TrySendUniform("material.shininess", material.GetShininess());
    }
}

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

void Object::Render(ShaderProgram& program)
{
    m_mesh->Apply();

    if (m_mesh->HasSubMesh())
    {
        for (int i = 0; i < m_mesh->GetSubMeshCount(); ++i)
        {
            SendMaterial(program, *m_materials[i]);
            Mesh::SubMesh subMesh = m_mesh->GetSubMesh(i);
            glDrawArrays(GL_TRIANGLES, subMesh.begin, subMesh.vertCount);
            GET_AND_HANDLE_GL_ERROR();
        }
    }
    else
    {
        if (m_materials.size() == 0)
        {
            SendMaterial(program, Material::GetDefaultMaterial());
        }
        else
        {
            SendMaterial(program, *m_materials[0]);
        }
        glDrawArrays(GL_TRIANGLES, 0, m_mesh->GetAttributeArray(0).GetAttributeCount());
        GET_AND_HANDLE_GL_ERROR();
    }
}

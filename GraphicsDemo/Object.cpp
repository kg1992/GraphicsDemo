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
    const int MaximumBoneCount = 128;
    const char* const JointTransformsUniformName = "jointTransforms[0]";

    void SendMaterial(ShaderProgram& program, Material& material)
    {
        if (material.GetAmbientMap() != 0)
        {
            glActiveTexture(GL_TEXTURE0);
            GET_AND_HANDLE_GL_ERROR();

            glBindTexture(GL_TEXTURE_2D, material.GetAmbientMap());
            GET_AND_HANDLE_GL_ERROR();

            program.TrySendUniform("material.ambientMap", 0);
        }
        program.TrySendUniform("material.ka", glm::vec3(material.GetAmbientColor()));

        if (material.GetDiffuseMap() != 0)
        {
            glActiveTexture(GL_TEXTURE1);
            GET_AND_HANDLE_GL_ERROR();

            glBindTexture(GL_TEXTURE_2D, material.GetDiffuseMap());
            GET_AND_HANDLE_GL_ERROR();

            program.TrySendUniform("material.diffuseMap", 1);
        }
        program.TrySendUniform("material.kd", glm::vec3(material.GetDiffuseColor()));

        if (material.GetSpecularMap() != 0)
        {
            glActiveTexture(GL_TEXTURE2);
            GET_AND_HANDLE_GL_ERROR();

            glBindTexture(GL_TEXTURE_2D, material.GetSpecularMap());
            GET_AND_HANDLE_GL_ERROR();

            program.TrySendUniform("material.specularMap", 2);
        }
        program.TrySendUniform("material.ks", material.GetSpecularColor());

        program.TrySendUniform("material.shininess", material.GetShininess());

        if (material.GetNormalMap() != 0)
        {
            glActiveTexture(GL_TEXTURE3);
            GET_AND_HANDLE_GL_ERROR();

            glBindTexture(GL_TEXTURE_2D, material.GetNormalMap());
            GET_AND_HANDLE_GL_ERROR();

            program.TrySendUniform("material.normalMap", 3);
        }
    }
}

Object::Object()
    : m_position()
    , m_scale(1,1,1)
    , m_rotation(glm::identity<glm::quat>())
    , m_mwMatrix(glm::identity<glm::mat4x4>())
    , m_meshes()
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
    m_mwMatrix = transformMatrix;
}

void Object::UpdateTransformMatrix()
{
    glm::mat4 i = glm::identity<glm::mat4x4>();
    glm::mat4 s = glm::scale(glm::identity<glm::mat4x4>(), m_scale);
    glm::mat4 r = glm::toMat4(m_rotation);
    glm::mat4 t = glm::translate(glm::identity<glm::mat4x4>(), m_position);
    m_mwMatrix = t * r * s;
}

void Object::Update()
{
    if (m_pAnimator)
    {
        m_pAnimator->Update(*this);
    }
}

const glm::mat4x4& Object::GetTransformMatrix()
{
    return m_mwMatrix;
}

void Object::AddMaterial(std::shared_ptr<Material> pMaterial)
{
    m_materials.push_back(pMaterial);
}

std::shared_ptr<Material> Object::GetMaterial(int index)
{
    return m_materials[index];
}

void Object::AddMesh(std::shared_ptr<Mesh> mesh)
{
    m_meshes.push_back(mesh);
}

std::shared_ptr<Mesh> Object::GetMesh(int index)
{
    return m_meshes[index];
}

void Object::Free()
{
    while (!m_materials.empty())
    {
        m_materials.back()->Free();
        m_materials.pop_back();
    }

    while( !m_meshes.empty())
    {
        m_meshes.back()->Free();
        m_meshes.pop_back();
    }

    m_pAnimator.reset();

    m_pSkeleton.reset();
}

void Object::Render(ShaderProgram& program)
{
    if (m_pSkeleton)
    {
        glm::mat4 boneTransforms[MaximumBoneCount];
        const int BoneCount = m_pSkeleton->GetBoneCount();
        for (int i = 0; i < BoneCount; ++i)
        {
            Bone& bone = m_pSkeleton->GetBoneByIndex(i);
            boneTransforms[i] = bone.GetAnimationTrasnform();
        }
        for (int i = BoneCount; i < MaximumBoneCount; ++i)
        {
            boneTransforms[i] = glm::identity<glm::mat4>();
        }

        program.TrySendUniform(JointTransformsUniformName, BoneCount, GL_FALSE, boneTransforms[0]);
    }

    for (auto& mesh : m_meshes)
    {
        mesh->Apply();

        if (mesh->HasSubMesh())
        {
            for (int i = 0; i < mesh->GetSubMeshCount(); ++i)
            {
                SendMaterial(program, *m_materials[i]);
                Mesh::SubMesh subMesh = mesh->GetSubMesh(i);
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
            glDrawArrays(GL_TRIANGLES, 0, mesh->GetAttributeArray(0).GetAttributeCount());
            GET_AND_HANDLE_GL_ERROR();
        }
    }
}

std::shared_ptr<Skeleton> Object::GetSkeleton()
{
    return m_pSkeleton;
}

void Object::SetSkeleton(std::shared_ptr<Skeleton> pSkeleton)
{
    m_pSkeleton = pSkeleton;
}

void Object::SetAnimator(std::shared_ptr<Animator> pAnimator)
{
    m_pAnimator = pAnimator;
}

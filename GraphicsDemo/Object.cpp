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
#include "Serialization.h"

namespace
{
    const int MaximumBoneCount = 128;
    const char* const JointTransformsUniformName = "uJointTransforms[0]";
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

int Object::GetMeshCount()
{
    return static_cast<int>(m_meshes.size());
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

void Object::Serialize(std::ostream& os)
{
    Serialization::Write(os, m_position);
    Serialization::Write(os, m_scale);
    Serialization::Write(os, m_rotation);

    // m_materials
    int materialCount = static_cast<int>(m_materials.size());
    Serialization::Write(os, materialCount);
    for (int i = 0; i < materialCount; ++i)
    {
        m_materials[i]->Serialize(os);
    }

    // m_meshes
    int meshCount = static_cast<int>(m_meshes.size());
    Serialization::Write(os, meshCount);
    for (int i = 0; i < meshCount; ++i)
    {
        m_meshes[i]->Serialize(os);
    }

    int skeletonCount = m_pSkeleton ? 1 : 0;
    Serialization::Write(os, skeletonCount);
    if (skeletonCount) {
        m_pSkeleton->Serialize(os);
    }

    int animatorCount = m_pAnimator ? 1 : 0;
    Serialization::Write(os, animatorCount);
    if (animatorCount) {
        m_pAnimator->Serialize(os);
    }
}

void Object::Deserialize(std::istream& is)
{
    Free();

    Serialization::Read(is, m_position);
    Serialization::Read(is, m_scale);
    Serialization::Read(is, m_rotation);

    int materialCount = 0;
    Serialization::Read(is, materialCount);
    for (int i = 0; i < materialCount; ++i)
    {
        std::shared_ptr<Material> pMaterial(new Material());
        pMaterial->Deserialize(is);
        AddMaterial(pMaterial);
    }

    int meshesCount = 0;
    Serialization::Read(is, meshesCount);
    for( int i = 0; i < meshesCount; ++i )
    {
        std::shared_ptr<Mesh> pMesh(new Mesh());
        pMesh->Deserialize(is);
        AddMesh(pMesh);
    }

    int skeletonCount;
    Serialization::Read(is, skeletonCount);
    if (skeletonCount)
    {
        m_pSkeleton.reset(new Skeleton());
        m_pSkeleton->Deserialize(is);
    }

    int animatorCount;
    Serialization::Read(is, animatorCount);
    if (animatorCount)
    {
        m_pAnimator.reset(new Animator());
        m_pAnimator->Deserialize(is);
    }

    UpdateTransformMatrix();
}

void Object::CopyTo(Object& dest)
{
    dest.m_position = m_position;
    dest.m_scale = m_scale;
    dest.m_rotation = m_rotation;
    // model to world transform matrix
    dest.m_mwMatrix = m_mwMatrix;

    dest.m_meshes = m_meshes;
    dest.m_materials = m_materials;
    if (m_pSkeleton)
    {
        dest.m_pSkeleton.reset(new Skeleton);
        m_pSkeleton->CopyTo(*dest.m_pSkeleton);
    }
    if (m_pAnimator)
    {
        dest.m_pAnimator.reset(new Animator);
        m_pAnimator->CopyTo(*dest.m_pAnimator);
    }
}

void Object::SendAnimationData(ShaderProgram& program)
{
    program.SendUniform("uAnimationEnabled", m_pSkeleton ? GL_TRUE : GL_FALSE);
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

        program.SendUniform(JointTransformsUniformName, BoneCount, GL_FALSE, boneTransforms[0]);
    }
}
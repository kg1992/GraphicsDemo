/*
    Bone.h

    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    References :
        https://www.youtube.com/watch?v=f3Cr8Yx3GGA

    Dependencies :
        glm - vector and matrix representation

    Bone class implementation
*/
#include "Common.h"
#include "Bone.h"
#include "Serialization.h"

Bone::Bone()
    : m_parent(DUMMY_PARENT_NODE_INDEX)
    , m_transform(glm::identity<glm::mat4>())
    , m_linkTransform(glm::identity<glm::mat4>())
    , m_invTransform(glm::identity<glm::mat4>())
    , m_invLinkTransform(glm::identity<glm::mat4>())
    , m_animationTransform(glm::identity<glm::mat4>())
{
}

void Bone::SetTransform(const glm::mat4 & transform)
{
    m_transform = transform;
    m_invTransform = glm::inverse(transform);
}

const glm::mat4 & Bone::GetTransform()
{
    return m_transform;
}

const glm::mat4 & Bone::GetInvTransform()
{
    return m_invTransform;
}

void Bone::SetLinkTransform(const glm::mat4 & linkTransform)
{
    m_linkTransform = linkTransform;
    m_invLinkTransform = glm::inverse(linkTransform);
}

const glm::mat4 & Bone::GetLinkTransform()
{
    return m_linkTransform;
}

const glm::mat4 & Bone::GetInvLinkTransform()
{
    return m_invLinkTransform;
}

int Bone::GetParent()
{
    return m_parent;
}

void Bone::SetParent(int parentIndex)
{
    m_parent = parentIndex;
}

void Bone::SetName(const char * name)
{
    m_name = name;
}

void Bone::SetName(const std::string & name)
{
    m_name = name;
}

const std::string & Bone::GetName() const
{
    return m_name;
}

void Bone::SetAnimationTransform(const glm::mat4 & mat)
{
    m_animationTransform = mat;
}

const glm::mat4 & Bone::GetAnimationTrasnform()
{
    return m_animationTransform;
}

void Bone::Serialize(std::ostream& os)
{
    Serialization::Write(os, m_parent);
    Serialization::Write(os, m_name);
    Serialization::Write(os, m_transform);
    Serialization::Write(os, m_animationTransform);
    Serialization::Write(os, m_invTransform);
    Serialization::Write(os, m_linkTransform);
    Serialization::Write(os, m_invLinkTransform);
}

void Bone::Deserialize(std::istream& is)
{
    Serialization::Read(is, m_parent);
    Serialization::Read(is, m_name);
    Serialization::Read(is, m_transform);
    Serialization::Read(is, m_animationTransform);
    Serialization::Read(is, m_invTransform);
    Serialization::Read(is, m_linkTransform);
    Serialization::Read(is, m_invLinkTransform);
}

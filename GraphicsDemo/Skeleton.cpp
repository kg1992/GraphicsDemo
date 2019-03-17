#include "Common.h"
#include "Skeleton.h"
#include "Serialization.h"

Skeleton::Skeleton()
{
}

Bone & Skeleton::GetRootBone()
{
    return m_bones[0];
}

void Skeleton::AddBone(const Bone & bone)
{
    m_bones.push_back(bone);
}

Bone & Skeleton::GetBone(const char * name)
{
    return *std::find_if(m_bones.begin(), m_bones.end(), [name](const Bone& bone)
    {
        return bone.GetName() == name;
    });
}

Bone & Skeleton::GetBoneByIndex(int index)
{
    return m_bones[index];
}

int Skeleton::GetBoneCount()
{
    return static_cast<int>(m_bones.size());
}

int Skeleton::FindBoneIndex(const char * name)
{
    std::vector<Bone>::iterator iter = std::find_if(m_bones.begin(), m_bones.end(), [name](const Bone& bone)
    {
        return bone.GetName() == name;
    });

    if (iter != m_bones.end())
        return static_cast<int>(iter - m_bones.begin());
    else
        return DUMMY_PARENT_NODE_INDEX;
}

int Skeleton::FindBoneIndex(const std::string & name)
{
    return FindBoneIndex(name.c_str());
}

void Skeleton::Serialize(std::ostream& os)
{
    int boneCount = static_cast<int>(m_bones.size());
    Serialization::Write(os, boneCount);
    for (int i = 0; i < boneCount; ++i)
    {
        m_bones[i].Serialize(os);
    }
}

void Skeleton::Deserialize(std::istream& is)
{
    int boneCount;
    Serialization::Read(is, boneCount);
    m_bones.reserve(boneCount);
    for (int i = 0; i < boneCount; ++i)
    {
        Bone bone;
        bone.Deserialize(is);
        m_bones.push_back(bone);
    }
}

void Skeleton::CopyTo(Skeleton& dest)
{
    dest.m_bones.assign(m_bones.begin(), m_bones.end());
}

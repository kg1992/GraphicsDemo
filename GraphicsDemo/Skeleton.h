#ifndef SKELETON_H_
#define SKELETON_H_

#include "Bone.h"

class Skeleton
{
public:
    enum { DUMMY_PARENT_NODE_INDEX = -1 };
    Skeleton()
    {
    }
    ~Skeleton()
    {
    }

    Bone& GetRootBone()
    {
        return m_bones[0];
    }

    void AddBone(const Bone& bone)
    {
        m_bones.push_back(bone);
    }

    Bone& GetBone(const char* name)
    {
        return *std::find_if(m_bones.begin(), m_bones.end(), [name](const Bone& bone)
        {
            return bone.GetName() == name;
        });
    }

    Bone& GetBoneByIndex(int index)
    {
        return m_bones[index];
    }

    int GetBoneCount()
    {
        return static_cast<int>(m_bones.size());
    }

    const int FindBoneIndex(const char* name)
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

    const int FindBoneIndex(const std::string& name)
    {
        return FindBoneIndex(name.c_str());
    }

private:
    std::vector<Bone> m_bones;
};

#endif

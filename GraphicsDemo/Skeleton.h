/*
    Skeleton.h

    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    References :
        https://www.youtube.com/watch?v=f3Cr8Yx3GGA
        https://www.youtube.com/channel/UCNm7HOHHI90oIc9SySfSqrw

    Skeleton class definition
*/
#ifndef SKELETON_H_
#define SKELETON_H_

#include "Bone.h"

class Skeleton
{
public:
    enum { DUMMY_PARENT_NODE_INDEX = -1 };

    Skeleton();
    
    Bone& GetRootBone();

    void AddBone(const Bone& bone);

    Bone& GetBone(const char* name);

    Bone& GetBoneByIndex(int index);

    int GetBoneCount();

    int FindBoneIndex(const char* name);

    int FindBoneIndex(const std::string& name);

    void Serialize(std::ostream& os);

    void Deserialize(std::istream& is);

private:
    std::vector<Bone> m_bones;
};

#endif

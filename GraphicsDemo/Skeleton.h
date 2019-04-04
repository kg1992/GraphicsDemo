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
    
    // The root bone is topmost bone that governs entire model.
    // Moving the root bone is like moving entire character by transation without morphing
    // It is common practice to place root bone at pelvis for human figure.
    Bone& GetRootBone();

    // Adds bone. Parent bone must be specified by index( Bone::SetParent )
    void AddBone(const Bone& bone);

    Bone& GetBone(const char* name);

    Bone& GetBoneByIndex(int index);

    int GetBoneCount();

    int FindBoneIndex(const char* name);

    int FindBoneIndex(const std::string& name);

    void Serialize(std::ostream& os);

    void Deserialize(std::istream& is);

    void CopyTo(Skeleton& dest);

private:
    // List of bones represent a Skeleton.
    // Each bone not only contains the original location of the bone placed by designer ( Bind Pose )
    // But also the mutated bone transform applied by 'Animator' object.
    std::vector<Bone> m_bones;
};

#endif

/*
    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    References :
        - https://www.youtube.com/watch?v=f3Cr8Yx3GGA

    Dependencies :
        glm - vector, matrix representation
*/
#include "Common.h"
#include "Animator.h"
#include "Object.h"
#include "Skeleton.h"
#include "Animation.h"
#include "Serialization.h"

Animator::Animator()
    : m_currentAnimation()
    , m_animationTime(0.f)
{
}

std::shared_ptr<Animation> Animator::GetCurrentAnimation()
{
    return m_currentAnimation;
}

void Animator::SetCurrentAnimation(std::shared_ptr<Animation> pAnimation)
{
    m_currentAnimation = pAnimation;
}

void Animator::Update(Object& object)
{
    if (m_currentAnimation == nullptr)
        return;

    std::map<std::string, glm::mat4> currentPose = CalcCurrentAnimationPose();
    std::shared_ptr<Skeleton> pSkeleton = object.GetSkeleton();
    if (pSkeleton)
    {
        ApplyPoseToBones(currentPose, pSkeleton);
    }

    IncreaseAnimationTime();
}

void Animator::Serialize(std::ostream& os) const
{
    m_currentAnimation->Serialize(os);

    Serialization::Write(os, m_animationTime);
}

void Animator::Deserialize(std::istream& is)
{
    m_currentAnimation.reset(new Animation(""));
    m_currentAnimation->Deserialize(is);

    Serialization::Read(is, m_animationTime);
}

void Animator::CopyTo(Animator& dest)
{
    dest.m_currentAnimation = m_currentAnimation;

    dest.m_animationTime = m_animationTime;
}

void Animator::IncreaseAnimationTime()
{
    m_animationTime += 0.016f;
    const float AnimationLength = m_currentAnimation->GetLength();
    if (m_animationTime > AnimationLength)
    {
        m_animationTime -= AnimationLength;
    }
}

std::map<std::string, glm::mat4> Animator::CalcCurrentAnimationPose()
{
    KeyFrame* pPrev = m_currentAnimation->GetKeyFrameBefore(m_animationTime);
    KeyFrame* pNext = m_currentAnimation->GetKeyFrameAfter(m_animationTime);

    float t = (m_animationTime - pPrev->GetTimeStamp()) / (pNext->GetTimeStamp() - pPrev->GetTimeStamp());
    return InterpolatePoses(*pPrev, *pNext, t);
}

std::map<std::string, glm::mat4> Animator::InterpolatePoses(KeyFrame & prev, KeyFrame & next, float t)
{
    std::map<std::string, glm::mat4> currentPose;

    // Pose is set of transforms for bone
    const std::map<std::string, BoneTransform>& NextPose = next.GetPose();
    for (auto& kv : prev.GetPose())
    {
        BoneTransform nextTransform = NextPose.at(kv.first);
        currentPose[kv.first] = BoneTransform::Interpolate(kv.second, nextTransform, t).GetTransformMatrix();
    }

    return currentPose;
}

void Animator::ApplyPoseToBones(std::map<std::string, glm::mat4>& pose, std::shared_ptr<Skeleton> pSkeleton)
{
    // Transform matrix of parent born is used in calculation of descendent nodes.
    // This map caches transform matrix per Bone
    std::unordered_map<Bone*, glm::mat4> globalMatrixCache;
    const int BoneCount = pSkeleton->GetBoneCount();
    for (int i = 0; i < BoneCount; ++i)
    {
        Bone* pBone = &pSkeleton->GetBoneByIndex(i);
        // Parent bone index. Root bone has parent id DUMMY_PARENT_NODE_INDEX
        int parentIndex = pBone->GetParent();
        assert(parentIndex < i);
        // Parent bone.
        Bone* pParent = pBone->GetParent() != Skeleton::DUMMY_PARENT_NODE_INDEX
            ? &pSkeleton->GetBoneByIndex(parentIndex)
            : nullptr;
        // Parent bone transform matrix
        const glm::mat4 ParentMatrix = pParent ? globalMatrixCache[pParent] : glm::identity<glm::mat4>();
        const glm::mat4& LocalTransform = pose[pBone->GetName()];
        const glm::mat4 CurrentTransform = ParentMatrix * LocalTransform;
        globalMatrixCache[pBone] = CurrentTransform;
        pBone->SetAnimationTransform(CurrentTransform * pBone->GetInvLinkTransform() * pBone->GetTransform());
    }
}

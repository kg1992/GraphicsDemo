#include "Common.h"
#include "Animator.h"
#include "Object.h"
#include "Skeleton.h"

Animator::Animator()
    : m_currentAnimation()
    , m_animationTime(0.f)
{
}

void Animator::AddAnimation(std::shared_ptr<Animation> pAnimation)
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

    auto nextPose = next.GetPose();
    for (auto& kv : prev.GetPose())
    {
        BoneTransform nextTransform = nextPose[kv.first];
        currentPose[kv.first] = BoneTransform::Interpolate(kv.second, nextTransform, t).GetTransformMatrix();
    }

    return currentPose;
}

void Animator::ApplyPoseToBones(std::map<std::string, glm::mat4>& pose, std::shared_ptr<Skeleton> pSkeleton)
{
    std::unordered_map<Bone*, glm::mat4> globalMatrixCache;
    for (int i = 0; i < pSkeleton->GetBoneCount(); ++i)
    {
        Bone* bone = &pSkeleton->GetBoneByIndex(i);
        int parentIndex = bone->GetParent();
        assert(parentIndex < i);
        Bone* parent = bone->GetParent() != Skeleton::DUMMY_PARENT_NODE_INDEX ? &pSkeleton->GetBoneByIndex(parentIndex) : nullptr;
        glm::mat4 parentMatrix = parent ? globalMatrixCache[parent] : glm::identity<glm::mat4>();
        glm::mat4& localTransform = pose[bone->GetName()];
        glm::mat4 currentTransform = parentMatrix * localTransform;
        globalMatrixCache[bone] = currentTransform;
        glm::mat4 animationTransform = currentTransform * bone->GetInvLinkTransform() * bone->GetTransform();
        bone->SetAnimationTransform(animationTransform);
    }
}

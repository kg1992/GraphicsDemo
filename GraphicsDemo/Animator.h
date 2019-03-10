#ifndef ANIMATOR_H_
#define ANIMATOR_H_

#include "Animation.h"

class Object;
class Skeleton;
class Bone;

class Animator
{
public:
    Animator();

    void AddAnimation(std::shared_ptr<Animation> pAnimation);

    void Update(Object& object);

private:
    std::shared_ptr<Animation> m_currentAnimation;
    float m_animationTime;

    void IncreaseAnimationTime();

    std::map<std::string, glm::mat4> CalcCurrentAnimationPose();

    std::map<std::string, glm::mat4> InterpolatePoses(KeyFrame& prev, KeyFrame& next, float t);

    void ApplyPoseToBones(std::map<std::string, glm::mat4>& pose, std::shared_ptr<Skeleton> pSkeleton);
};

#endif

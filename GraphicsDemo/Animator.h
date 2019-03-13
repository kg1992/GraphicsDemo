/*
    Animator.h

    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    References :
        - https://www.youtube.com/watch?v=f3Cr8Yx3GGA

    Dependencies :
        glm - vector, matrix representation

    Animator class definition.
*/
#ifndef ANIMATOR_H_
#define ANIMATOR_H_

class Object;
class Skeleton;
class Bone;
class Animation;
class KeyFrame;

//
// Animator class manages internal timer, to read in 'KeyFrame's of 'Animatioin' to interpolate pose at the time.
// 'Animator' class also propagates bone/joint transform along the skeleton given by 'Object' in Animator::Update() call.
//
// usage:
//  std::shared_ptr<Animation> pAnimation(...)/ // Create and Initialize 'Animation'
//  Animator* animator = new Animator(); 
//  animator.AddAnimation(animation); // Initialize 'Animator'
//  
//  ...
//
//  aniamtor.Update(object); // every frame
//
class Animator
{
public:
    Animator();

    // Gets current animation that is playing.
    std::shared_ptr<Animation> GetCurrentAnimation();

    // Sets current animation to be played.
    void SetCurrentAnimation(std::shared_ptr<Animation> pAnimation);

    // Invoke every frame to play animation.s
    void Update(Object& object);

private:
    // Currently playing animation.
    std::shared_ptr<Animation> m_currentAnimation;

    // Currently playing time of the animation
    float m_animationTime;

    void IncreaseAnimationTime();

    std::map<std::string, glm::mat4> CalcCurrentAnimationPose();

    std::map<std::string, glm::mat4> InterpolatePoses(KeyFrame& prev, KeyFrame& next, float t);

    void ApplyPoseToBones(std::map<std::string, glm::mat4>& pose, std::shared_ptr<Skeleton> pSkeleton);
};

#endif

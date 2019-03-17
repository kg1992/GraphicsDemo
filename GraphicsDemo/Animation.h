/*
    Animation.h

    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    References :
        - https://www.youtube.com/watch?v=f3Cr8Yx3GGA

    Dependencies :
        glm - vector, matrix representation

    BoneTransform struct definition.
    KeyFrame, Animation class definition.
*/
#ifndef ANIMATION_H_
#define ANIMATION_H_

#include "KeyFrame.h"

//
// class Animation
//
// class 'Animation' represents a single shot of animation.
// Animation is represented by multiple instances of 'KeyFrame' over timeline.
// Internally class 'Animation' stores array of 'KeyFrame' in time ascending order.
// Each 'KeyFrame' has timestamp set. To get time stamp of a KeyFrame, call KeyFarme::GetTimeStamp()
//
class Animation
{
public:
    Animation(const std::string& name = "");

    const std::string& GetName();

    void SetName(const std::string& name);

    // Adds a keyframe.
    // Todo : sort insert
    void AddKeyFrame(KeyFrame keyFrame);

    // Gets the keyframe placed before the given 'timeStamp'
    // If the 'timeStamp' went out of boundary of Animation length, it just returns the first or the last frame.
    KeyFrame* GetKeyFrameBefore(float timeStamp);

    // Gets the keyframe placed after the given 'timeStamp'
    // If the 'timeStamp' went out of boundary of Animation length, it just returns the first or the last frame.
    KeyFrame* GetKeyFrameAfter(float timeStamp);

    // Sets length of the animation in seconds
    void SetLength(float length);

    // Gets length of the animation in seconds
    float GetLength();

    void Serialize(std::ostream& os) const;

    void Deserialize(std::istream& is);

private:
    std::string m_name;
    std::vector<KeyFrame> m_keyFrames;
    float m_length;
};

#endif

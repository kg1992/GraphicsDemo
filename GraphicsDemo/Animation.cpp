/*
    Animation.cpp

    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    References :
        - https://www.youtube.com/watch?v=f3Cr8Yx3GGA

    Dependencies :
        glm - vector, matrix representation

    Animation class imeplementation.
*/
#include "Common.h"
#include "Animation.h"

Animation::Animation(const std::string & name)
    : m_name(name)
{
}

const std::string& Animation::GetName()
{
    return m_name;
}

void Animation::AddKeyFrame(KeyFrame keyFrame)
{
    m_keyFrames.push_back(keyFrame);
}

KeyFrame * Animation::GetKeyFrameBefore(float timeStamp)
{
    if (timeStamp > m_length)
        return &m_keyFrames.back();

    for (std::vector<KeyFrame>::reverse_iterator iter = m_keyFrames.rbegin(); iter != m_keyFrames.rend(); ++iter)
    {
        if (iter->GetTimeStamp() < timeStamp)
        {
            return &*iter;
        }
    }

    return &m_keyFrames.front();
}

KeyFrame * Animation::GetKeyFrameAfter(float timeStamp)
{
    for (KeyFrame& keyFrame : m_keyFrames)
    {
        if (keyFrame.GetTimeStamp() > timeStamp)
        {
            return &keyFrame;
        }
    }
    return &m_keyFrames.back();
}

void Animation::SetLength(float length)
{
    m_length = length;
}

float Animation::GetLength()
{
    return m_length;
}

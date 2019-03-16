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
#include "Serialization.h"

Animation::Animation(const std::string & name)
    : m_name(name)
{
}

const std::string& Animation::GetName()
{
    return m_name;
}

void Animation::SetName(const std::string& name)
{
    m_name = name;
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

void Animation::Serialize(std::ostream& os) const
{
    Serialization::Write(os, m_name);
    int keyFrameCount = static_cast<int>(m_keyFrames.size());
    Serialization::Write(os, keyFrameCount);
    for (int i = 0; i < keyFrameCount; ++i)
    {
        m_keyFrames[i].Serialize(os);
    }
    Serialization::Write(os, m_length);
}

void Animation::Deserialize(std::istream& is)
{
    Serialization::Read(is, m_name);
    int keyFrameCount;
    Serialization::Read(is, keyFrameCount);
    m_keyFrames.resize(keyFrameCount);
    for (int i = 0; i < keyFrameCount; ++i)
    {
        m_keyFrames[i].Deserialize(is);
    }
    Serialization::Read(is, m_length);
}

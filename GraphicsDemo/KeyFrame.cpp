#include "Common.h"
#include "KeyFrame.h"
#include "Serialization.h"

std::map<std::string, BoneTransform>& KeyFrame::GetPose()
{
    return m_pose;
}

void KeyFrame::AddPose(const char * boneName, BoneTransform transform)
{
    m_pose[boneName] = transform;
}

void KeyFrame::SetTimestamp(float timeStamp)
{
    m_timeStamp = timeStamp;
}

float KeyFrame::GetTimeStamp()
{
    return m_timeStamp;
}

void KeyFrame::Serialize(std::ostream& os) const
{
    Serialization::Write(os, m_timeStamp);

    int poseSize = static_cast<int>(m_pose.size());
    Serialization::Write(os, poseSize);
    for (std::map<std::string, BoneTransform>::const_iterator iter = m_pose.begin(); iter != m_pose.end(); ++iter)
    {
        Serialization::Write(os, iter->first);
        Serialization::Write(os, iter->second);
    }
}

void KeyFrame::Deserialize(std::istream& is)
{
    Serialization::Read(is, m_timeStamp);

    int poseSize;
    Serialization::Read(is, poseSize);
    for( int i = 0; i < poseSize; ++i )
    {
        std::string key;
        Serialization::Read(is, key);
        Serialization::Read(is, m_pose[key]);
    }
}

#ifndef ANIMATION_H_
#define ANIMATION_H_

struct BoneTransform
{
    glm::mat4 GetTransformMatrix()
    {
        glm::mat4 r = glm::toMat4(rotation);
        glm::mat4 t = glm::translate(glm::identity<glm::mat4>() , position);
        return t * r;
    }

    static BoneTransform Interpolate(BoneTransform& a, BoneTransform& b, float t)
    {
        BoneTransform transform;
        transform.position = glm::lerp(a.position, b.position, t);
        transform.rotation = glm::slerp(a.rotation, b.rotation, t);
        return transform;
    }

    glm::vec3 position;
    glm::quat rotation;
};

class KeyFrame
{
public:
    std::map<std::string, BoneTransform>& GetPose()
    {
        return m_pose;
    }

    void AddPose(const char* boneName, BoneTransform transform)
    {
        m_pose[boneName] = transform;
    }

    void SetTimestamp(float timeStamp)
    {
        m_timeStamp = timeStamp;
    }

    float GetTimeStamp()
    {
        return m_timeStamp;
    }

private:
    float m_timeStamp;
    std::map<std::string, BoneTransform> m_pose;
};

class Animation
{
public:
    Animation(const std::string& name)
        : m_name(name)
    {
    } 

    void AddKeyFrame(KeyFrame keyFrame)
    {
        m_keyFrames.push_back(keyFrame);
    }

    KeyFrame* GetKeyFrameBefore(float timeStamp)
    {
        if (timeStamp > m_length)
            return &m_keyFrames.back();

        for( std::vector<KeyFrame>::reverse_iterator iter = m_keyFrames.rbegin(); iter != m_keyFrames.rend(); ++iter)
        {
            if (iter->GetTimeStamp() < timeStamp)
            {
                return &*iter;
            }
        }

        return &m_keyFrames.front();
    }

    KeyFrame* GetKeyFrameAfter(float timeStamp)
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

    void SetLength(float length)
    {
        m_length = length;
    }

    float GetLength()
    {
        return m_length;
    }

private:
    std::vector<KeyFrame> m_keyFrames;
    std::string m_name;
    float m_length;
};

#endif

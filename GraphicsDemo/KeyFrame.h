/*
    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)
*/
#ifndef KEY_FRAME_H_
#define KEY_FRAME_H_

struct BoneTransform
{
    glm::mat4 GetTransformMatrix()
    {
        glm::mat4 r = glm::toMat4(rotation);
        glm::mat4 t = glm::translate(glm::identity<glm::mat4>(), position);
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
    std::map<std::string, BoneTransform>& GetPose();

    void AddPose(const char* boneName, BoneTransform transform);

    void SetTimestamp(float timeStamp);

    float GetTimeStamp();

    void Serialize(std::ostream& os) const;

    void Deserialize(std::istream& is);

private:
    float m_timeStamp;
    std::map<std::string, BoneTransform> m_pose;
};

#endif

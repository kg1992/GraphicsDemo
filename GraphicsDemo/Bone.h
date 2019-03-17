/*
    Bone.h

    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    References :
        https://www.youtube.com/watch?v=f3Cr8Yx3GGA

    Dependencies :
        glm - vector and matrix representation

    Bone class definition
*/
#ifndef BONE_H_
#define BONE_H_

class Bone
{
public:
    enum { DUMMY_PARENT_NODE_INDEX = -1 };

    Bone();

    void SetTransform(const glm::mat4& transform);

    const glm::mat4& GetTransform();

    const glm::mat4& GetInvTransform();

    void SetLinkTransform(const glm::mat4& linkTransform);

    const glm::mat4& GetLinkTransform();

    const glm::mat4& GetInvLinkTransform();

    int GetParent();

    void SetParent(int parentIndex);

    void SetName(const char* name);

    void SetName(const std::string& name);

    const std::string& GetName() const;

    void SetAnimationTransform(const glm::mat4& mat);

    const glm::mat4& GetAnimationTrasnform();

    void Serialize(std::ostream& os);

    void Deserialize(std::istream& is);

private:
    int m_parent;
    std::string m_name;
    // Joint Transform that sends vertex to animated location in model space
    glm::mat4 m_transform;    
    // Animation Traget Transform
    glm::mat4 m_animationTransform;
    glm::mat4 m_invTransform;
    glm::mat4 m_linkTransform;
    glm::mat4 m_invLinkTransform;
};

#endif
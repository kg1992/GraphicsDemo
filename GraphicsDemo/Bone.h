#ifndef BONE_H_
#define BONE_H_

class Bone
{
public:
    enum { DUMMY_PARENT_NODE_INDEX = -1 };

    Bone()
        : m_parent(DUMMY_PARENT_NODE_INDEX)
        , m_transform(glm::identity<glm::mat4>())
        , m_linkTransform(glm::identity<glm::mat4>())
        , m_invTransform(glm::identity<glm::mat4>())
        , m_invLinkTransform(glm::identity<glm::mat4>())
        , m_animationTransform(glm::identity<glm::mat4>())
    {
    }

    void SetTransform(const glm::mat4& transform)
    {
        m_transform = transform;
        m_invTransform = glm::inverse(transform);
    }

    const glm::mat4& GetTransform()
    {
        return m_transform;
    }

    const glm::mat4& GetInvTransform()
    {
        return m_invTransform;
    }

    void SetLinkTransform(const glm::mat4& linkTransform)
    {
        m_linkTransform = linkTransform;
        m_invLinkTransform = glm::inverse(linkTransform);
    }

    const glm::mat4& GetLinkTransform()
    {
        return m_linkTransform;
    }

    const glm::mat4& GetInvLinkTransform()
    {
        return m_invLinkTransform;
    }

    int GetParent()
    {
        return m_parent;
    }

    void SetParent(int parentIndex)
    {
        m_parent = parentIndex;
    }

    void SetName(const char* name)
    {
        m_name = name;
    }

    void SetName(const std::string& name)
    {
        m_name = name;
    }

    const std::string& GetName() const
    {
        return m_name;
    }

    void SetAnimationTransform(const glm::mat4& mat)
    {
        m_animationTransform = mat;
    }

    const glm::mat4& GetAnimationTrasnform() 
    {
        return m_animationTransform;
    }

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
#include "Common.h"
#include "PerspectiveCamera.h"

void PerspectiveCamera::SetPosition(const glm::vec3 & pos)
{
    m_position = pos;

    UpdateEyeMatrix();
}

void PerspectiveCamera::SetPosition(float x, float y, float z)
{
    m_position.x = x;
    m_position.y = y;
    m_position.z = z;

    UpdateEyeMatrix();
}

const glm::vec3 & PerspectiveCamera::GetPosition()
{
    return m_position;
}

void PerspectiveCamera::SetCenter(const glm::vec3 & pos)
{
    m_center = pos;

    UpdateEyeMatrix();
}

void PerspectiveCamera::SetCenter(float x, float y, float z)
{
    m_center.x = x;
    m_center.y = y;
    m_center.z = z;

    UpdateEyeMatrix();
}

const glm::vec3 & PerspectiveCamera::GetCenter()
{
    return m_center;
}

void PerspectiveCamera::SetReferenceUp(glm::vec3 up)
{
    m_up = up;

    UpdateEyeMatrix();
}

void PerspectiveCamera::SetReferenceUp(float x, float y, float z)
{
    m_up.x = x;
    m_up.y = y;
    m_up.z = z;

    UpdateEyeMatrix();
}

const glm::vec3 & PerspectiveCamera::GetReferenceUp()
{
    return m_up;
}

void PerspectiveCamera::LookAt(const glm::vec3 & pos, const glm::vec3 & center, const glm::vec3 & up)
{
    m_position = pos;
    m_center = center;
    m_up = up;

    UpdateEyeMatrix();
}

void PerspectiveCamera::UpdateEyeMatrix()
{
    m_eyeMatrix = glm::lookAt(m_position, m_center, m_up);
}

void PerspectiveCamera::SetFrustum(float l, float r, float b, float t, float n, float f)
{
    m_left = l;
    m_right = r;
    m_bottom = b;
    m_top = t;
    m_near = n;
    m_far = f;
    m_projectionMatrix = glm::frustum(l, r, b, t, n, f);
}

void PerspectiveCamera::SetFrustum(float fovy, float aspectRatio, float n, float f)
{
    const float Tangent = std::tan(fovy);
    const float H = Tangent * n;
    const float W = aspectRatio * H;
    m_left = -W;
    m_right = W;
    m_bottom = -H;
    m_top = H;
    m_near = n;
    m_far = f;
    m_projectionMatrix = glm::perspective(fovy, aspectRatio, n, f);
}

float PerspectiveCamera::GetNearTop()
{
    return m_top;
}

float PerspectiveCamera::GetNearRight()
{
    return m_right;
}

float PerspectiveCamera::GetNear()
{
    return m_near;
}

Frustum PerspectiveCamera::GetFrustum()
{
    return Frustum(m_right * 2
        , m_top * 2
        , m_far / m_near * m_right * 2
        , m_far / m_near * m_top * 2
        , m_near
        , m_far);
}

void PerspectiveCamera::MoveBy(glm::vec3 disp)
{
    m_position += disp;
    m_center += disp;
    UpdateEyeMatrix();
}

const glm::mat4x4& PerspectiveCamera::EyeMatrix()
{
    return m_eyeMatrix;
}

const glm::mat4x4& PerspectiveCamera::ProjectionMatrix()
{
    return m_projectionMatrix;
}

glm::vec3 PerspectiveCamera::Forward()
{
    return glm::normalize(m_center - m_position);
}

glm::vec3 PerspectiveCamera::Up()
{
    glm::vec3 f = Forward();
    return m_up - glm::dot(m_up, f) * f;
}

float PerspectiveCamera::GetAspectRatio()
{
    return m_right / m_top;
}

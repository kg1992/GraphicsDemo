#include "Common.h"
#include "Camera.h"

void Camera::SetPosition(const glm::vec3 & pos)
{
    m_position = pos;

    UpdateEyeMatrix();
}

void Camera::SetPosition(float x, float y, float z)
{
    m_position.x = x;
    m_position.y = y;
    m_position.z = z;

    UpdateEyeMatrix();
}

const glm::vec3 & Camera::GetPosition()
{
    return m_position;
}

void Camera::SetCenter(const glm::vec3 & pos)
{
    m_center = pos;

    UpdateEyeMatrix();
}

void Camera::SetCenter(float x, float y, float z)
{
    m_center.x = x;
    m_center.y = y;
    m_center.z = z;

    UpdateEyeMatrix();
}

const glm::vec3 & Camera::GetCenter()
{
    return m_center;
}

void Camera::SetUp(glm::vec3 up)
{
    m_up = up;

    UpdateEyeMatrix();
}

void Camera::SetUp(float x, float y, float z)
{
    m_up.x = x;
    m_up.y = y;
    m_up.z = z;

    UpdateEyeMatrix();
}

const glm::vec3 & Camera::GetUp()
{
    return m_up;
}

void Camera::LookAt(const glm::vec3 & pos, const glm::vec3 & center, const glm::vec3 & up)
{
    m_position = pos;
    m_center = center;
    m_up = up;

    UpdateEyeMatrix();
}

void Camera::UpdateEyeMatrix()
{
    m_eyeMatrix = glm::lookAt(m_position, m_center, m_up);
}

void Camera::SetFrustum(float l, float r, float b, float t, float n, float f)
{
    m_projectionMatrix = glm::frustum(l, r, b, t, n, f);
}

void Camera::SetFrustum(float fovy, float w, float h, float n, float f)
{
    m_projectionMatrix = glm::perspective(fovy, w / h, n, f);
}

void Camera::SetFrustum(float fovy, float aspectRatio, float n, float f)
{
    m_projectionMatrix = glm::perspective(fovy, aspectRatio, n, f);
}

void Camera::MoveBy(glm::vec3 disp)
{
    m_position += disp;
    m_center += disp;
    UpdateEyeMatrix();
}

const glm::mat4x4& Camera::EyeMatrix()
{
    return m_eyeMatrix;
}

const glm::mat4x4& Camera::ProjectionMatrix()
{
    return m_projectionMatrix;
}

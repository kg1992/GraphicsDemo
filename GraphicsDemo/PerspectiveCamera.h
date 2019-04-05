#ifndef PERSPECTIVE_CAMERA_H_
#define PERSPECTIVE_CAMERA_H_

#include "Geometry.h"

class PerspectiveCamera
{
public:
    void SetPosition(const glm::vec3& pos);

    void SetPosition(float x, float y, float z);

    const glm::vec3& GetPosition();

    void SetCenter(const glm::vec3& pos);

    void SetCenter(float x, float y, float z);

    const glm::vec3& GetCenter();

    void SetReferenceUp(glm::vec3 up);

    void SetReferenceUp(float x, float y, float z);

    const glm::vec3& GetReferenceUp();

    void LookAt(const glm::vec3& pos, const glm::vec3& center, const glm::vec3& up);

    void UpdateEyeMatrix();

    void SetFrustum(float l, float r, float b, float t, float n, float f);

    void SetFrustum(float fovy, float aspectRatio, float n, float f);

    float GetNearTop();

    float GetNearRight();

    float GetNear();

    // Generates Frustum Geometry that represents Frustum of the camera in view space.
    Frustum GetFrustum();

    void MoveBy(glm::vec3 disp);

    const glm::mat4x4& EyeMatrix();

    const glm::mat4x4& ProjectionMatrix();

    glm::vec3 Forward();

    glm::vec3 Up();

    float GetAspectRatio();

private:
    float m_right;
    float m_left;
    float m_top;
    float m_bottom;
    float m_near;
    float m_far;
    glm::vec3 m_position;
    glm::vec3 m_center;
    glm::vec3 m_up;
    glm::mat4x4 m_eyeMatrix;
    glm::mat4x4 m_projectionMatrix;
};

#endif

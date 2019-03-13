/*
    Lights.h

    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    PointLight, SpotLight, DirecitonalLight definition.
*/
#ifndef LIGHTS_H_
#define LIGHTS_H_

class PointLight
{
public:
    glm::vec3 position;
    glm::vec3 la;
    glm::vec3 ld;
    glm::vec3 ls;
};

class SpotLight
{
public:
    const glm::vec3& GetPosition();

    void SetPosition(const glm::vec3& position);

    const glm::vec3& GetAmbient();

    void SetAmbientColor(const glm::vec3& ambient);

    const glm::vec3& GetDiffuse();

    void SetDiffuse(const glm::vec3& diffuse);

    const glm::vec3& GetDirection();

    void SetDirection(const glm::vec3& direction);

    float GetExponent();

    void SetExponent(float exponent);

    float GetCutoff();

    void SetCutoff(float cutoff);
    

private:
    glm::vec3 m_position;
    glm::vec3 m_la;
    glm::vec3 m_ld;
    glm::vec3 m_ls;
    glm::vec3 m_direction;
    float m_exponent;
    float m_cutoff;
};

#endif

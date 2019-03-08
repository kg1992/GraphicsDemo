/*
    Lights.cpp

    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    PointLight, SpotLight, DirecitonalLight implmentation.
*/
#include "Common.h"
#include "Lights.h"

const glm::vec3 & SpotLight::GetPosition()
{
    return m_position;
}

void SpotLight::SetPosition(const glm::vec3 & position)
{
    m_position = position;
}

const glm::vec3 & SpotLight::GetAmbient()
{
    return m_la;
}

void SpotLight::SetAmbientColor(const glm::vec3 & ambient)
{
    m_la = ambient;
}

const glm::vec3 & SpotLight::GetDiffuse()
{
    return m_ld;
}

void SpotLight::SetDiffuse(const glm::vec3 & diffuse)
{
    m_ld = diffuse;
}

const glm::vec3 & SpotLight::GetDirection()
{
    return m_direction;
}

void SpotLight::SetDirection(const glm::vec3 & direction)
{
    m_direction = direction;
}

float SpotLight::GetExponent()
{
    return m_exponent;
}

void SpotLight::SetExponent(float exponent)
{
    m_exponent = exponent;
}

float SpotLight::GetCutoff()
{
    return m_cutoff;
}

void SpotLight::SetCutoff(float cutoff)
{
    m_cutoff = cutoff;
}

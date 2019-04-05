/*
    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    PointLight, SpotLight, DirecitonalLight implmentation.
*/
#include "Common.h"
#include "Lights.h"
#include "Errors.h"

const glm::vec3 & Light::GetPosition()
{
    return m_position;
}

void Light::SetPosition(const glm::vec3 & position)
{
    m_position = position;
}

const glm::vec3 & Light::GetAmbient()
{
    return m_la;
}

void Light::SetAmbientColor(const glm::vec3 & ambient)
{
    m_la = ambient;
}

const glm::vec3 & Light::GetDiffuse()
{
    return m_ld;
}

void Light::SetDiffuse(const glm::vec3 & diffuse)
{
    m_ld = diffuse;
}

const glm::vec3 & Light::GetDirection()
{
    return m_direction;
}

void Light::SetDirection(const glm::vec3 & direction)
{
    m_direction = direction;
}

float Light::GetExponent()
{
    return m_exponent;
}

void Light::SetExponent(float exponent)
{
    m_exponent = exponent;
}

float Light::GetCutoff()
{
    return m_cutoff;
}

void Light::SetCutoff(float cutoff)
{
    m_cutoff = cutoff;
}

DirectionalLight::DirectionalLight()
    : m_castShadow(false)
{
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::SetCastShadow(bool castShadow)
{
    m_castShadow = castShadow;
}

bool DirectionalLight::GetCastShadow()
{
    return m_castShadow;
}

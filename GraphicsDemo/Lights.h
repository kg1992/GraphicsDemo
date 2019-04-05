/*
    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    PointLight, SpotLight, DirecitonalLight definition.
*/
#ifndef LIGHTS_H_
#define LIGHTS_H_

#include "ShadowMap.h"

class Light
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
    glm::vec3 m_direction;
    glm::vec3 m_la;
    glm::vec3 m_ld;
    glm::vec3 m_ls;
    float m_exponent;
    float m_cutoff;
};

class PointLight : public Light
{
};

class DirectionalLight : public Light
{
public:
    DirectionalLight();

    ~DirectionalLight();

    void SetCastShadow(bool castShadow);

    bool GetCastShadow();

    void SetShadowMap(std::shared_ptr<ShadowMap2D> shadowMap)
    {
        m_shadowMap = shadowMap;
    }

    std::shared_ptr<ShadowMap2D> GetShadowMap()
    {
        return m_shadowMap;
    }

private:
    bool m_castShadow;
    std::shared_ptr<ShadowMap2D> m_shadowMap;
    
};

class SpotLight : public Light
{
public:
    
    

private:
};

#endif

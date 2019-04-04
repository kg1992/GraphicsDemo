/*
    Scene.h

    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    class Scene is composition of entities that maeks a single 'scenary'

    Todo:
        Later GraphicsDemo will have multiple scene and switch between them.
*/
#include "Common.h"
#include "Scene.h"
#include "Object.h"
#include "Serialization.h"

void Scene::Init() {}

void Scene::Update()
{
    for (std::shared_ptr<Object> pObject : m_objects)
    {
        pObject->Update();
    }
}

void Scene::Free() {}

Camera & Scene::GetCamera() { return m_camera; }

std::shared_ptr<Object> Scene::GetSceneObject(int index) {
    return m_objects[index];
}

int Scene::GetSceneObjectCount()
{
    return static_cast<int>(m_objects.size());
}

void Scene::AddSceneObject(std::shared_ptr<Object> pObject)
{
    m_objects.push_back(pObject);
}

PointLight & Scene::GetPointLight(int index)
{
    return m_pointLights[index];
}

int Scene::GetPointLightCount()
{
    return static_cast<int>(m_pointLights.size());
}

SpotLight & Scene::GetSpotLight(int index)
{
    return m_spotLights[index];
}

int Scene::GetSpotLightCount()
{
    return static_cast<int>(m_spotLights.size());
}

void Scene::Serialize(std::ostream& os) const
{
    int objectCount = static_cast<int>(m_objects.size());
    Serialization::Write(os, objectCount);
    for (int i = 0; i < objectCount; ++i)
    {
        m_objects[i]->Serialize(os);
    }
    
    int pointLightCount = static_cast<int>(m_pointLights.size());
    Serialization::Write(os, pointLightCount);
    for (int i = 0; i < pointLightCount; ++i)
    {
        Serialization::Write(os, m_pointLights[i]);
    }

    int spotLightCount = static_cast<int>(m_spotLights.size());
    Serialization::Write(os, spotLightCount);
    for (int i = 0; i < spotLightCount; ++i)
    {
        Serialization::Write(os, m_spotLights[i]);
    }

    Serialization::Write(os, m_camera);
}

void Scene::Deserialize(std::istream& is)
{
    int objectCount;
    Serialization::Read(is, objectCount);
    m_objects.resize(objectCount);
    for (int i = 0; i < objectCount; ++i)
    {
        m_objects[i].reset(new Object());
        m_objects[i]->Deserialize(is);
    }

    int pointLightCount;
    Serialization::Read(is, pointLightCount);
    m_pointLights.resize(pointLightCount);
    for (int i = 0; i < pointLightCount; ++i)
    {
        Serialization::Read(is, m_pointLights[i]);
    }

    int spotLightCount;
    Serialization::Read(is, spotLightCount);
    m_spotLights.resize(spotLightCount);
    for (int i = 0; i < spotLightCount; ++i)
    {
        Serialization::Read(is, m_spotLights[i]);
    }

    Serialization::Read(is, m_camera);
}

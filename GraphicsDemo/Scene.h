/*
    Scene.h

    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    class Scene is composition of entities that maeks a single 'scenary'

    Todo:
        Later GraphicsDemo will have multiple scene and switch between them.
*/
#ifndef SCENE_H_
#define SCENE_H_

#include "Lights.h"
#include "Camera.h"

class Object;

class Scene
{
public:
    virtual void Init() {}

    virtual void Update();

    virtual void Free() {}

    Camera& GetCamera() { return m_camera;  }

    std::shared_ptr<Object> GetSceneObject(int index) {
        return m_objects[index];
    }

    int GetSceneObjectCount()
    {
        return static_cast<int>(m_objects.size());
    }

    void AddSceneObject(std::shared_ptr<Object> pObject)
    {
        m_objects.push_back(pObject);
    }

    PointLight& GetPointLight(int index)
    {
        return m_pointLights[index];
    }

    int GetPointLightCount()
    {
        return static_cast<int>(m_pointLights.size());
    }

    SpotLight& GetSpotLight(int index)
    {
        return m_spotLights[index];
    }

    int GetSpotLightCount()
    {
        return static_cast<int>(m_spotLights.size());
    }

    void Serialize(std::ostream& os) const;

    void Deserialize(std::istream& is);

protected:
    std::vector<std::shared_ptr<Object>> m_objects;
    std::vector<PointLight> m_pointLights;
    std::vector<SpotLight> m_spotLights;
    Camera m_camera;
};

#endif

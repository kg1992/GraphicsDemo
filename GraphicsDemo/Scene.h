/*
    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    class Scene is composition of entities that maeks a single 'scenary'

    Todo:
        Later GraphicsDemo will have multiple scene and switch between them.
*/
#ifndef SCENE_H_
#define SCENE_H_

#include "Lights.h"
#include "PerspectiveCamera.h"

class Object;

class Scene
{
public:
    virtual void Init();

    virtual void Update();

    virtual void Free();

    int GetCameraCount();

    PerspectiveCamera& GetCamera(int index = 0);

    std::shared_ptr<Object> GetSceneObject(int index);

    int GetSceneObjectCount();

    void AddSceneObject(std::shared_ptr<Object> pObject);

    PointLight& GetPointLight(int index);

    int GetPointLightCount();

    SpotLight& GetSpotLight(int index);

    int GetSpotLightCount();

    DirectionalLight& GetDirectionalLight(int index);

    int GetDirectionalLightCount();

    void Serialize(std::ostream& os) const;

    void Deserialize(std::istream& is);

protected:
    std::vector<std::shared_ptr<Object>> m_objects;
    std::vector<DirectionalLight> m_directionalLights;
    std::vector<PointLight> m_pointLights;
    std::vector<SpotLight> m_spotLights;
    std::vector<PerspectiveCamera> m_cameras;
};

#endif

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

void Scene::Update()
{
    for (std::shared_ptr<Object> pObject : m_objects)
    {
        pObject->Update();
    }
}
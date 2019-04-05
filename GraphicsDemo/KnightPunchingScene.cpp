/*
    KnightPunchingScene.cpp

    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    KnightPunchingScene class implementation.
    The scene spawns a ground and a knight who is constantly punching in the air.
*/
#include "Common.h"
#include "KnightPunchingScene.h"
#include "Object.h"
#include "Material.h"
#include "FbxLoader.h"
#include "System.h"
#include "Geometry.h"

namespace
{
    const int MaximumLightCount = 5;
    float s_spotSpeed = 4.0f;
    float s_speedX = -1.0;
    float s_speedZ = 1.1618f;
    float s_spotAngle = 0.0f;
}

KnightPunchingScene::KnightPunchingScene()
    : Scene()
{
    {
        PerspectiveCamera camera;
        camera.LookAt(glm::vec3(50, 50, 50), glm::vec3(), glm::vec3(0, 1, 0));
        m_cameras.push_back(camera);
    }

    {
        PerspectiveCamera camera;
        camera.LookAt(glm::vec3(-50, 50, 50), glm::vec3(), glm::vec3(0, 1, 0));
        m_cameras.push_back(camera);
    }
}

void KnightPunchingScene::Init()
{
    {
        DirectionalLight light;
        light.SetDirection(glm::normalize(glm::vec3(1.f, -1.f, 1.f)));
        light.SetAmbientColor(glm::vec3(0.1f, 0.1f, 0.1f));
        light.SetDiffuse(glm::vec3(1.0f, 1.0f, 1.0f));
        light.SetCastShadow(true);
        std::shared_ptr<ShadowMap2D> pShadowMap(new ShadowMap2D());

        const float Maximum = 600.0f;
        const float Boundary = 500.0f;

        pShadowMap->Init(1024, 1024, Boundary * 2, Boundary * 2, Maximum);
        light.SetShadowMap(pShadowMap);
        m_directionalLights.push_back(light);
    }

    // Load Objecrts
    FbxLoader loader;
    std::shared_ptr<Object> obj;

    // Punching Knight
    loader.Load("./Resources/Punching Knight.fbx");
    obj = loader.GetObjectByIndex(0);
    m_objects.push_back(obj);

    // Coast
    loader.Load("./Resources/Terrain/Coast.fbx");
    obj = loader.GetObjectByIndex(0);
    m_objects.push_back(obj);

    //// Add ground
    //AddGround();

    std::shared_ptr<Mesh> pCube = GenerateCube();
    for (int i = 0; i < 12; ++i)
    {
        std::shared_ptr<Object> pObject(new Object);
        
        pObject->AddMesh(pCube);

        const float Radius = 200.0f;
        const float YStep = 50.0f;
        const glm::vec3 Position = glm::vec3(
            Radius * std::cos(i*0.5f),
            i * YStep,
            Radius * std::sin(i*0.5f));
        const float Scale = 100.0f;
        pObject->SetPosition(Position);
        pObject->SetScale(glm::vec3(Scale));
        
        m_objects.push_back(pObject);
    }
}

void KnightPunchingScene::Update()
{
    float time = System::Instance()->CurrentTime() * 0.25f;
    float x = cosf(time);
    float y = -1.f;
    float z = sinf(time);
    m_directionalLights[0].SetDirection(glm::normalize(glm::vec3(x, y, z)));

    this->Scene::Update();
}

void KnightPunchingScene::AddGround()
{
    // Ground object
    std::shared_ptr<Object> pGround(new Object);

    // Ground mesh
    pGround->AddMesh(GeneratePlane());

    // Scale to cover large area
    pGround->SetScale(glm::vec3(100, 1, 100));
    m_objects.push_back(pGround);

    // Material
    std::shared_ptr<Material> pMaterial(new Material());
    pMaterial->SetAmbientColor(1.f, 1.f, 1.f, 1.f);
    pMaterial->SetDiffuseColor(0.4f, 0.26f, 0.13f, 1.f);
    pMaterial->SetSpecularColor(0.f, 0.f, 1.f, 1.f);
    pMaterial->SetShininess(50.0f);

    pGround->AddMaterial(pMaterial);
}

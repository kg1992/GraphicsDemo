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

std::shared_ptr<Mesh> GeneratePlane();

namespace
{
    const int ActiveLightCount = 1;
    const int MaximumLightCount = 5;
    float s_spotSpeed = 4.0f;
    float s_speedX = -1.0;
    float s_speedZ = 1.1618f;
    float s_spotAngle = 0.0f;
}

KnightPunchingScene::KnightPunchingScene()
    : Scene()
{
}

void KnightPunchingScene::Init()
{
    PrepareLights();

    // Initial m_camera location.
    m_camera.LookAt(glm::vec3(50, 50, 50), glm::vec3(), glm::vec3(0, 1, 0));

    // Load Objecrts
    FbxLoader loader;
    std::shared_ptr<Object> obj;

    // Punching Knight
    loader.Load("./Resources/Punching Knight.fbx");
    obj = loader.GetObjectByIndex(0);
    m_objects.push_back(obj);

    // Add ground
    AddGround();
}

void KnightPunchingScene::Update()
{
    float time = System::Instance()->CurrentTime() * 0.25f;
    float radius = 100.f;
    float x = cosf(time) * radius;
    float y = 150.f;
    float z = sinf(time) * radius;
    m_pointLights[0].position = glm::vec4(x, y, z, 1.0f);

    //////////

    glm::vec3 d = m_spotLights[0].GetDirection();
    glm::vec3 longitudialTangent = glm::cross(d, glm::vec3(0, 1, 0));
    glm::vec3 latittudialTangent = glm::cross(d, longitudialTangent);
    const float SecPerFrame = 0.016f;
    const float g = 9.8f;
    s_speedX += g * -d.x* SecPerFrame;
    d.x += s_speedX * SecPerFrame;
    s_speedZ += g * -d.z * SecPerFrame;
    d.z += s_speedZ * SecPerFrame;
    m_spotLights[0].SetDirection(glm::normalize(d));

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

void KnightPunchingScene::PrepareLights()
{
    PointLight pl;

    pl.la = glm::vec3(0.1f, 0.1f, 0.1f);
    pl.ld = glm::vec3(1.0f, 1.0f, 1.0f);
    pl.ls = glm::vec3(1.0f, 1.0f, 1.0f);

    m_pointLights.push_back(pl);

    //////////

    m_spotLights.resize(1);
    m_spotLights[0].SetPosition(glm::vec3(.0f, 100.0f, .0f));
    m_spotLights[0].SetDirection(glm::normalize(glm::vec3(0.0f, -1.0f, -0.3f)));
    m_spotLights[0].SetDiffuse(glm::vec3(1.0f, 1.0f, 1.0f));
    m_spotLights[0].SetAmbientColor(glm::vec3(.0f, .0f, .0f));
    m_spotLights[0].SetCutoff(glm::radians(50.f));
    m_spotLights[0].SetExponent(50.0f);
}

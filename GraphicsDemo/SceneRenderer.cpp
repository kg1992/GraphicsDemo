/*
    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    class is main renderer that renders scene with correct animation and shading.
*/
#include "Common.h"
#include "SceneRenderer.h"
#include "ShaderPrograms.h"
#include "Object.h"
#include "Lights.h"
#include "Camera.h"
#include "Scene.h"

namespace
{
    const int ActiveLightCount = 1;
    const int MaximumLightCount = 5;
}

void SceneRenderer::Render(std::shared_ptr<Scene> pScene)
{
    ShaderProgram program = ShaderPrograms::s_phong;

    program.Use();

    for (int i = 0; i < pScene->GetPointLightCount(); ++i)
    {
        SendPointLight(pScene->GetCamera(), program, i, pScene->GetPointLight(i));
    }

    // SendSpotLight(pScene->GetCamera(), program, pScene->GetSpotLight(0));

    for( int i = 0; i < pScene->GetSceneObjectCount(); ++i )
    {
        RenderObject(pScene->GetCamera(), program, *pScene->GetSceneObject(i));
    }
}

void SceneRenderer::SendPointLight(Camera& camera, ShaderProgram& program, int index, PointLight& light)
{
    std::stringstream ss;

    ss << "light[" << index << "].position";
    program.SendUniform(ss.str().c_str(), camera.EyeMatrix() * glm::vec4(light.position, 1));
    std::stringstream().swap(ss);

    ss << "light[" << index << "].la";
    program.SendUniform(ss.str().c_str(), light.la);
    std::stringstream().swap(ss);

    ss << "light[" << index << "].l";
    program.SendUniform(ss.str().c_str(), light.ld);
    std::stringstream().swap(ss);
}

void SceneRenderer::SendSpotLight(Camera& camera, ShaderProgram& program, SpotLight& spotLight)
{
    glm::mat4 viewMatrix = camera.EyeMatrix();
    glm::vec3 position = viewMatrix * glm::vec4(spotLight.GetPosition(), 1);
    program.SendUniform("spot.position", position);
    program.SendUniform("spot.l", spotLight.GetDiffuse());
    program.SendUniform("spot.la", spotLight.GetAmbient());

    glm::mat3 normalMatrix = glm::mat3(glm::vec3(viewMatrix[0]), glm::vec3(viewMatrix[1]), glm::vec3(viewMatrix[2]));
    glm::vec3 direction = normalMatrix * spotLight.GetDirection();
    program.SendUniform("spot.direction", direction);
    program.SendUniform("spot.exponent", spotLight.GetExponent());
    program.SendUniform("spot.cutoff", spotLight.GetCutoff());
}

void SceneRenderer::SendMatrices(Camera& camera, ShaderProgram& program, Object& object)
{
    glm::mat4x4 mvMatrix = camera.EyeMatrix() * object.GetTransformMatrix();
    program.SendUniform("mvMatrix", 1, false, mvMatrix);
    glm::mat3x3 normalMatrix = glm::transpose(glm::inverse(mvMatrix));
    program.SendUniform("normalMatrix", 1, false, normalMatrix);
    program.SendUniform("projMatrix", 1, false, camera.ProjectionMatrix());
}

void SceneRenderer::RenderObject(Camera& camera, ShaderProgram& program, Object& object)
{
    program.Use();

    SendMatrices(camera, program, object);

    object.Render(program);
}
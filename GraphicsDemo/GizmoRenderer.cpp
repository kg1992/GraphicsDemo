/*
    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    GizmoRenderer class implementation.
    The class helps visualize entities that has no mesh ( such as lights, camera )
*/
#include "Common.h"
#include "GizmoRenderer.h"
#include "ShaderPrograms.h"
#include "Lights.h"
#include "Errors.h"
#include "Scene.h"
#include "Object.h"

namespace
{
    const int ActiveLightCount = 1;
    const int MaximumLightCount = 5;
}

void GizmoRenderer::Render(std::shared_ptr<Scene> pScene)
{
    DrawPointLights(pScene);
    DrawSpotLight(pScene);
    DrawObjectCenter(pScene);
}

void GizmoRenderer::DrawPointLights(std::shared_ptr<Scene> pScene)
{
    auto& program = ShaderPrograms::s_pointLight;

    for (int i = 0; i < ActiveLightCount; ++i)
    {
        PointLight& pointLight = pScene->GetPointLight(i);

        program.Use();
        program.SendUniform("wCenterPos", glm::vec4(pointLight.position, 1));
        program.SendUniform("wScale", 5.0f, 5.0f);
        program.SendUniform("wvMatrix", 1, false, pScene->GetCamera().EyeMatrix());
        program.SendUniform("projMatrix", 1, false, pScene->GetCamera().ProjectionMatrix());

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        GET_AND_HANDLE_GL_ERROR();
    }
}

void GizmoRenderer::DrawSpotLight(std::shared_ptr<Scene> pScene)
{
    auto& program = ShaderPrograms::s_pointLight;

    program.Use();
    program.SendUniform("wCenterPos", glm::vec4(pScene->GetSpotLight(0).GetPosition(), 1));
    program.SendUniform("wScale", 5.0f, 5.0f);
    program.SendUniform("wvMatrix", 1, false, pScene->GetCamera().EyeMatrix());
    program.SendUniform("projMatrix", 1, false, pScene->GetCamera().ProjectionMatrix());

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    GET_AND_HANDLE_GL_ERROR();
}

void GizmoRenderer::DrawObjectCenter(std::shared_ptr<Scene> pScene)
{
    ShaderProgram& program = ShaderPrograms::s_axes;

    program.Use();

    for (int i = 0; i < pScene->GetSceneObjectCount(); ++i)
    {
        std::shared_ptr<Object> pObject = pScene->GetSceneObject(i);

        SendMatrices(pScene->GetCamera(), program, *pObject);

        glm::vec3 position = pObject->GetPosition();

        glDrawArrays(GL_LINES, 0, 6);
    }
}

void GizmoRenderer::SendMatrices(Camera& camera, ShaderProgram& program, Object& object)
{
    program.SendUniform("projMatrix", 1, false, camera.ProjectionMatrix());
}

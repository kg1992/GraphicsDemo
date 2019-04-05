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
#include "Geometry.h"

namespace
{
    const int ActiveLightCount = 1;
    const int MaximumLightCount = 5;
}

void GizmoRenderer::Render(PerspectiveCamera& camera, std::shared_ptr<Scene> pScene)
{
    DrawPointLights(camera, pScene);
    DrawSpotLight(camera, pScene);
    DrawObjectCenter(camera, pScene);

    {

        ////////// World space visualization //////////

        ShaderProgram& program = ShaderPrograms::s_axes;

        program.Use();

        const float D = glm::length(camera.GetPosition());
        const float H = D * camera.GetNearTop() / camera.GetNear();
        program.SendUniform("mvMatrix", 1, false, camera.EyeMatrix() * glm::scale(glm::vec3(H * 0.5f)));
        program.SendUniform("projMatrix", 1, false, camera.ProjectionMatrix());

        glDrawArrays(GL_LINES, 0, 6);

    }

    DrawPerspectiveCamera(camera, pScene);
}

void GizmoRenderer::DrawPointLights(PerspectiveCamera& camera, std::shared_ptr<Scene> pScene)
{
    auto& program = ShaderPrograms::s_pointLight;

    for (int i = 0; i < pScene->GetPointLightCount(); ++i)
    {
        PointLight& pointLight = pScene->GetPointLight(i);

        program.Use();
        program.SendUniform("wCenterPos", glm::vec4(pointLight.GetPosition(), 1));
        program.SendUniform("wScale", 5.0f, 5.0f);
        program.SendUniform("wvMatrix", 1, false, camera.EyeMatrix());
        program.SendUniform("projMatrix", 1, false, camera.ProjectionMatrix());

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        GET_AND_HANDLE_GL_ERROR();
    }
}

void GizmoRenderer::DrawSpotLight(PerspectiveCamera& camera, std::shared_ptr<Scene> pScene)
{
    auto& program = ShaderPrograms::s_pointLight;

    for (int i = 0; i < pScene->GetSpotLightCount(); ++i)
    {

        program.Use();
        program.SendUniform("wCenterPos", glm::vec4(pScene->GetSpotLight(i).GetPosition(), 1));
        program.SendUniform("wScale", 5.0f, 5.0f);
        program.SendUniform("wvMatrix", 1, false, camera.EyeMatrix());
        program.SendUniform("projMatrix", 1, false, camera.ProjectionMatrix());

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        GET_AND_HANDLE_GL_ERROR();
    }
}

void GizmoRenderer::DrawObjectCenter(PerspectiveCamera& camera, std::shared_ptr<Scene> pScene)
{
    ShaderProgram& program = ShaderPrograms::s_axes;

    program.Use();

    for (int i = 0; i < pScene->GetSceneObjectCount(); ++i)
    {
        std::shared_ptr<Object> pObject = pScene->GetSceneObject(i);

        SendMatrices(camera, program, *pObject);

        glDrawArrays(GL_LINES, 0, 6);
    }
}

void GizmoRenderer::DrawPerspectiveCamera(PerspectiveCamera& camera, std::shared_ptr<Scene> pScene)
{
    auto& program = ShaderPrograms::s_position;

    for (int i = 0; i < pScene->GetCameraCount(); ++i)
    {
        PerspectiveCamera& curr = pScene->GetCamera(i);

        Frustum frustum = curr.GetFrustum();

        program.Use();

        GLuint vbo;
        glGenBuffers(1, &vbo);
        GET_AND_HANDLE_GL_ERROR();
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        GET_AND_HANDLE_GL_ERROR();
        std::vector<glm::vec3> buffer;
        const int EdgeCount = frustum.GetEdgeCount();
        const int VertexCount = EdgeCount * 2;
        buffer.resize(VertexCount);
        for (int i = 0; i < EdgeCount; ++i)
        {
            Frustum::Edge edge = frustum.GetEdge(i);
            buffer.push_back(frustum.GetVertex(edge[0]));
            buffer.push_back(frustum.GetVertex(edge[1]));
        }
        glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(glm::vec3), buffer.data(), GL_STATIC_DRAW);
        GET_AND_HANDLE_GL_ERROR();

        glEnableVertexAttribArray(0);
        GET_AND_HANDLE_GL_ERROR();

        glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);
        GET_AND_HANDLE_GL_ERROR();

        program.SendUniform("mvMatrix", 1, false, curr.EyeMatrix() * glm::inverse(camera.EyeMatrix()));
        program.SendUniform("projMatrix", 1, false, curr.ProjectionMatrix());

        glDrawArrays(GL_LINES, 0, VertexCount);
        GET_AND_HANDLE_GL_ERROR();

        glDeleteBuffers(1, &vbo);
        GET_AND_HANDLE_GL_ERROR();
    }
}

void GizmoRenderer::SendMatrices(PerspectiveCamera& camera, ShaderProgram& program, Object& object)
{
    program.SendUniform("projMatrix", 1, false, camera.ProjectionMatrix());
}

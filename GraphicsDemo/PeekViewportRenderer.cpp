/*
    PeekViewportRenderer.cpp

    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    The class helps visualize attributes by drawing scene with colorized attribute on the bottom of the screen.
*/
#include "Common.h"
#include "PeekViewportRenderer.h"
#include "ShaderPrograms.h"
#include "ShaderProgram.h"
#include "Object.h"
#include "Scene.h"
#include "Errors.h"

void PeekViewportRenderer::Render(std::shared_ptr<Scene> pScene)
{
    unsigned int peekViewWidth = m_clientWidth / 8;
    unsigned int peekViewHeight = m_clientHeight / 8;
    ShaderProgram programs[] = { ShaderPrograms::s_position,
                                 ShaderPrograms::s_uv,
                                 ShaderPrograms::s_normal,
                                 ShaderPrograms::s_normalMap };

    for (int i = 0; i < _countof(programs); ++i)
    {
        programs[i].Use();

        glViewport(peekViewWidth * i, 0, peekViewWidth, peekViewHeight);
        GET_AND_HANDLE_GL_ERROR();

        for (int j = 0; j < pScene->GetSceneObjectCount(); ++j)
        {
            RenderObject(pScene->GetCamera(), programs[i], *pScene->GetSceneObject(j));
        }
    }
}

void PeekViewportRenderer::RenderObject(Camera& camera, ShaderProgram& program, Object& object)
{
    SendMatrices(camera, program, object);

    object.Render(program);
}

void PeekViewportRenderer::SendMatrices(Camera& camera, ShaderProgram& program, Object& object)
{
    program.SendUniform("mwMatrix", 1, false, object.GetTransformMatrix());
    glm::mat4x4 mvMatrix = camera.EyeMatrix() * object.GetTransformMatrix();
    program.SendUniform("mvMatrix", 1, false, mvMatrix);
    program.SendUniform("projMatrix", 1, false, camera.ProjectionMatrix());
}
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
#include "Mesh.h"

namespace
{
    const int MaxViewportCount = 8;
}

void PeekViewportRenderer::Render(std::shared_ptr<Scene> pScene)
{
    // viewport width
    unsigned int peekViewWidth = m_clientWidth / MaxViewportCount;
    unsigned int peekViewHeight = m_clientHeight / MaxViewportCount;
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

void PeekViewportRenderer::RenderObject(PerspectiveCamera& camera, ShaderProgram& program, Object& object)
{
    SendMatrices(camera, program, object);

    for (int i = 0; i < object.GetMeshCount(); ++i)
    {
        std::shared_ptr<Mesh> pMesh = object.GetMesh(i);
        pMesh->Apply();

        for (int j = 0; j < pMesh->GetSubMeshCount(); ++j)
        {
            Mesh::SubMesh subMesh = pMesh->GetSubMesh(j);
            glDrawArrays(GL_TRIANGLES, subMesh.begin, subMesh.vertCount);
            GET_AND_HANDLE_GL_ERROR();
        }
    }
}

void PeekViewportRenderer::SendMatrices(PerspectiveCamera& camera, ShaderProgram& program, Object& object)
{
    program.SendUniform("mwMatrix", 1, false, object.GetTransformMatrix());
    glm::mat4x4 mvMatrix = camera.EyeMatrix() * object.GetTransformMatrix();
    program.SendUniform("mvMatrix", 1, false, mvMatrix);
    program.SendUniform("projMatrix", 1, false, camera.ProjectionMatrix());
}
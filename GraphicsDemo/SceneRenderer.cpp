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
#include "Material.h"
#include "Mesh.h"
#include "Errors.h"

namespace
{
    const int ActiveLightCount = 1;
    const int MaximumLightCount = 5;

    void SendMaterial(ShaderProgram& program, Material& material)
    {
        if (material.GetAmbientMap() != 0)
        {
            glActiveTexture(GL_TEXTURE0);
            GET_AND_HANDLE_GL_ERROR();

            glBindTexture(GL_TEXTURE_2D, material.GetAmbientMap());
            GET_AND_HANDLE_GL_ERROR();

            program.SendUniform("uMaterial.ambientMap", 0);
        }
        program.SendUniform("uMaterial.ka", glm::vec3(material.GetAmbientColor()));

        if (material.GetDiffuseMap() != 0)
        {
            glActiveTexture(GL_TEXTURE1);
            GET_AND_HANDLE_GL_ERROR();

            glBindTexture(GL_TEXTURE_2D, material.GetDiffuseMap());
            GET_AND_HANDLE_GL_ERROR();

            program.SendUniform("uMaterial.diffuseMap", 1);
        }
        program.SendUniform("uMaterial.kd", glm::vec3(material.GetDiffuseColor()));

        if (material.GetSpecularMap() != 0)
        {
            glActiveTexture(GL_TEXTURE2);
            GET_AND_HANDLE_GL_ERROR();

            glBindTexture(GL_TEXTURE_2D, material.GetSpecularMap());
            GET_AND_HANDLE_GL_ERROR();

            program.SendUniform("uMaterial.specularMap", 2);
        }
        program.SendUniform("uMaterial.ks", glm::vec3(material.GetSpecularColor()));

        program.SendUniform("uMaterial.shininess", material.GetShininess());

        if (material.GetNormalMap() != 0)
        {
            glActiveTexture(GL_TEXTURE3);
            GET_AND_HANDLE_GL_ERROR();

            glBindTexture(GL_TEXTURE_2D, material.GetNormalMap());
            GET_AND_HANDLE_GL_ERROR();

            program.SendUniform("uMaterial.normalMap", 3);
        }

        program.SendUniform("uNormalMapEnabled", material.GetNormalMap() != 0);
    }
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
        Camera& camera = pScene->GetCamera();
        std::shared_ptr<Object> pObject = pScene->GetSceneObject(i);
        RenderObject(camera, program, *pObject);
    }
}

void SceneRenderer::SendPointLight(Camera& camera, ShaderProgram& program, int index, PointLight& light)
{
    std::stringstream ss;

    ss << "uLights[" << index << "].position";
    program.SendUniform(ss.str().c_str(), camera.EyeMatrix() * glm::vec4(light.position, 1));
    std::stringstream().swap(ss);

    ss << "uLights[" << index << "].la";
    program.SendUniform(ss.str().c_str(), light.la);
    std::stringstream().swap(ss);

    ss << "uLights[" << index << "].l";
    program.SendUniform(ss.str().c_str(), light.ld);
    std::stringstream().swap(ss);
}

void SceneRenderer::SendSpotLight(Camera& camera, ShaderProgram& program, SpotLight& spotLight)
{
    glm::mat4 viewMatrix = camera.EyeMatrix();
    glm::vec3 position = viewMatrix * glm::vec4(spotLight.GetPosition(), 1);
    program.SendUniform("uSpot.position", position);
    program.SendUniform("uSpot.l", spotLight.GetDiffuse());
    program.SendUniform("uSpot.la", spotLight.GetAmbient());

    glm::mat3 normalMatrix = glm::mat3(glm::vec3(viewMatrix[0]), glm::vec3(viewMatrix[1]), glm::vec3(viewMatrix[2]));
    glm::vec3 direction = normalMatrix * spotLight.GetDirection();
    program.SendUniform("uSpot.direction", direction);
    program.SendUniform("uSpot.exponent", spotLight.GetExponent());
    program.SendUniform("uSpot.cutoff", spotLight.GetCutoff());
}

void SceneRenderer::SendMatrices(Camera& camera, ShaderProgram& program, Object& object)
{
    glm::mat4x4 mvMatrix = camera.EyeMatrix() * object.GetTransformMatrix();
    program.SendUniform("uMvMatrix", 1, false, mvMatrix);
    glm::mat3x3 normalMatrix = glm::transpose(glm::inverse(mvMatrix));
    program.SendUniform("uNormalMatrix", 1, false, normalMatrix);
    program.SendUniform("uVpMatrix", 1, false, camera.ProjectionMatrix());
}

void SceneRenderer::RenderObject(Camera& camera, ShaderProgram& program, Object& object)
{
    program.Use();

    SendMatrices(camera, program, object);

    object.SendAnimationData(program);

    for( int k = 0; k < object.GetMeshCount(); ++k )
    {
        std::shared_ptr<Mesh> mesh(object.GetMesh(k));
        mesh->Apply();

        for (int i = 0; i < mesh->GetSubMeshCount(); ++i)
        {
            SendMaterial(program, *object.GetMaterial(i));
            Mesh::SubMesh subMesh = mesh->GetSubMesh(i);
            glDrawArrays(GL_TRIANGLES, subMesh.begin, subMesh.vertCount);
            GET_AND_HANDLE_GL_ERROR();
        }
    }
}
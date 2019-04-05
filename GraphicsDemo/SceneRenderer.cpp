/*
    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    class is main renderer that renders scene with correct animation and shading.
*/
#include "Common.h"
#include "SceneRenderer.h"
#include "ShaderPrograms.h"
#include "Object.h"
#include "Lights.h"
#include "PerspectiveCamera.h"
#include "Scene.h"
#include "Material.h"
#include "Mesh.h"
#include "Errors.h"
#include "System.h"

namespace
{
    const int MaximumLightCount = 5;
    const int MaximumDirectionalLightCount = 5;
    const int MaximumSpotLightCount = 5;
}

void SceneRenderer::Render(PerspectiveCamera& camera, std::shared_ptr<Scene> pScene)
{
    m_program.Use();

    glActiveTexture(GL_TEXTURE11);
    GET_AND_HANDLE_GL_ERROR();
    GLuint depthTexture = pScene->GetDirectionalLight(0).GetShadowMap()->GetDepthTexture();
    assert(depthTexture != 0);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    GET_AND_HANDLE_GL_ERROR();
    m_program.SendUniform("uShadowMap", 11);

    m_program.SendUniform("uShadowMapRender", GL_FALSE);

    const int PointLightCount = std::min(MaximumLightCount
        , pScene->GetPointLightCount());
    m_program.SendUniform("uActivePointLightCount", PointLightCount);
    for (int i = 0; i < PointLightCount; ++i)
    {
        SendPointLight(camera, i, pScene->GetPointLight(i));
    }

    const int DirectionalLightCount = std::min(MaximumDirectionalLightCount
        , pScene->GetDirectionalLightCount());
    m_program.SendUniform("uActiveDirectionalLightCount", DirectionalLightCount);
    for (int i = 0; i < DirectionalLightCount; ++i)
    {
        SendDirectionalLight(camera, i, pScene->GetDirectionalLight(i));
    }

    // m_program.SendUniform("uTime", System::Instance()->CurrentTime());

    for( int i = 0; i < pScene->GetSceneObjectCount(); ++i )
    {
        std::shared_ptr<Object> pObject = pScene->GetSceneObject(i);
        RenderObject(camera, *pObject);
    }
}

void SceneRenderer::SetRendererProgram(ShaderProgram program)
{
    m_program = program;
}

void SceneRenderer::SendPointLight(PerspectiveCamera& camera, int index, PointLight& light)
{
    std::stringstream ss;

    ss << "uLights[" << index << "].position";
    m_program.SendUniform(ss.str().c_str(), camera.EyeMatrix() * glm::vec4(light.GetPosition(), 1));
    std::stringstream().swap(ss);

    ss << "uLights[" << index << "].la";
    m_program.SendUniform(ss.str().c_str(), light.GetAmbient());
    std::stringstream().swap(ss);

    ss << "uLights[" << index << "].l";
    m_program.SendUniform(ss.str().c_str(), light.GetDiffuse());
    std::stringstream().swap(ss);
}

void SceneRenderer::SendDirectionalLight(PerspectiveCamera& camera, int index, DirectionalLight& light)
{
    std::stringstream ss;

    const std::string UniformName = "uDirectionalLights";

    ss << UniformName << "[" << index << "].castShadow";
    m_program.SendUniform(ss.str().c_str(), light.GetCastShadow());
    std::stringstream().swap(ss);

    glActiveTexture(GL_TEXTURE10);
    GET_AND_HANDLE_GL_ERROR();
    glBindTexture(GL_TEXTURE_2D, light.GetShadowMap()->GetDepthTexture());
    GET_AND_HANDLE_GL_ERROR();
    ss << UniformName << "[" << index << "].shadowMap";
    m_program.SendUniform(ss.str().c_str(), 10);
    std::stringstream().swap(ss);

    const float Distance = 600.0f;
    const float Boundary = 600.0f;
    ss << UniformName << "[" << index << "].shadowMatrix";
    const glm::mat4 V = glm::lookAtRH(-light.GetDirection(), glm::vec3(), glm::vec3(0, 1, 0));
    const glm::mat4 P = glm::orthoRH(-Boundary, Boundary, -Boundary, Boundary, -Distance, Distance);
    const glm::mat4 B(
        .5f, .0f, .0f, .0f,
        .0f, .5f, .0f, .0f,
        .0f, .0f, .5f, .0f,
        .5f, .5f, .5f, 1.f);
    const glm::mat4 ShadowMatrix = B * P * V;
    m_program.SendUniform(ss.str().c_str(), 1, false, ShadowMatrix);
    std::stringstream().swap(ss);
    
    ss << UniformName << "[" << index << "].direction";
    const glm::mat4 NormalMatrix = glm::transpose(glm::inverse(camera.EyeMatrix()));
    const glm::vec4 ViewSpaceDirection = NormalMatrix * glm::vec4(light.GetDirection(), 0);
    m_program.SendUniform(ss.str().c_str(), glm::vec3(ViewSpaceDirection));
    std::stringstream().swap(ss);

    ss << UniformName << "[" << index << "].la";
    m_program.SendUniform(ss.str().c_str(), light.GetAmbient());
    std::stringstream().swap(ss);

    ss << UniformName << "[" << index << "].l";
    m_program.SendUniform(ss.str().c_str(), light.GetDiffuse());
    std::stringstream().swap(ss);
}

void SceneRenderer::SendSpotLight(PerspectiveCamera& camera, SpotLight& spotLight)
{
    glm::mat4 viewMatrix = camera.EyeMatrix();
    glm::vec3 position = viewMatrix * glm::vec4(spotLight.GetPosition(), 1);
    m_program.SendUniform("uSpot.position", position);
    m_program.SendUniform("uSpot.l", spotLight.GetDiffuse());
    m_program.SendUniform("uSpot.la", spotLight.GetAmbient());

    glm::mat3 normalMatrix = glm::mat3(glm::vec3(viewMatrix[0]), glm::vec3(viewMatrix[1]), glm::vec3(viewMatrix[2]));
    glm::vec3 direction = normalMatrix * spotLight.GetDirection();
    m_program.SendUniform("uSpot.direction", direction);
    m_program.SendUniform("uSpot.exponent", spotLight.GetExponent());
    m_program.SendUniform("uSpot.cutoff", spotLight.GetCutoff());
}

void SceneRenderer::SendMatrices(PerspectiveCamera& camera, Object& object)
{
    m_program.SendUniform("uMwMatrix", 1, false, object.GetTransformMatrix());
    glm::mat4x4 mvMatrix = camera.EyeMatrix() * object.GetTransformMatrix();
    m_program.SendUniform("uMvMatrix", 1, false, mvMatrix);
    glm::mat3x3 normalMatrix = glm::transpose(glm::inverse(mvMatrix));
    m_program.SendUniform("uNormalMatrix", 1, false, normalMatrix);
    m_program.SendUniform("uVpMatrix", 1, false, camera.ProjectionMatrix());
}

void SceneRenderer::RenderObject(PerspectiveCamera& camera, Object& object)
{
    m_program.Use();

    SendMatrices(camera, object);

    object.SendAnimationData(m_program);

    const int MaterialCount = object.GetMaterialCount();

    for( int k = 0; k < object.GetMeshCount(); ++k )
    {
        std::shared_ptr<Mesh> mesh(object.GetMesh(k));
        mesh->Apply();

        for (int i = 0; i < mesh->GetSubMeshCount(); ++i)
        {
            SendMaterial(i < MaterialCount ? *object.GetMaterial(i) : Material::GetDefaultMaterial());
            Mesh::SubMesh subMesh = mesh->GetSubMesh(i);
            glDrawArrays(GL_TRIANGLES, subMesh.begin, subMesh.vertCount);
            GET_AND_HANDLE_GL_ERROR();
        }
    }
}

void SceneRenderer::SendMaterial(Material& material)
{
    glActiveTexture(GL_TEXTURE0);
    GET_AND_HANDLE_GL_ERROR();

    glBindTexture(GL_TEXTURE_2D, material.GetAmbientMap());
    GET_AND_HANDLE_GL_ERROR();

    m_program.SendUniform("uMaterial.ambientMap", 0);
    m_program.SendUniform("uMaterial.useAmbientMap", material.GetAmbientMap() != 0);
    m_program.SendUniform("uMaterial.ka", glm::vec3(material.GetAmbientColor()));

    glActiveTexture(GL_TEXTURE1);
    GET_AND_HANDLE_GL_ERROR();

    glBindTexture(GL_TEXTURE_2D, material.GetDiffuseMap());
    GET_AND_HANDLE_GL_ERROR();

    m_program.SendUniform("uMaterial.diffuseMap", 1);
    m_program.SendUniform("uMaterial.useDiffuseMap", material.GetDiffuseMap() != 0);
    m_program.SendUniform("uMaterial.kd", glm::vec3(material.GetDiffuseColor()));

    glActiveTexture(GL_TEXTURE2);
    GET_AND_HANDLE_GL_ERROR();

    glBindTexture(GL_TEXTURE_2D, material.GetSpecularMap());
    GET_AND_HANDLE_GL_ERROR();

    m_program.SendUniform("uMaterial.specularMap", 2);
    m_program.SendUniform("uMaterial.useSpecularMap", material.GetSpecularMap() != 0);
    m_program.SendUniform("uMaterial.ks", glm::vec3(material.GetSpecularColor()));

    m_program.SendUniform("uMaterial.shininess", material.GetShininess());

    if (material.GetNormalMap() != 0)
    {
        glActiveTexture(GL_TEXTURE3);
        GET_AND_HANDLE_GL_ERROR();

        glBindTexture(GL_TEXTURE_2D, material.GetNormalMap());
        GET_AND_HANDLE_GL_ERROR();

        m_program.SendUniform("uMaterial.normalMap", 3);
    }

    m_program.SendUniform("uNormalMapEnabled", material.GetNormalMap() != 0);
}

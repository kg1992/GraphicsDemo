/*
    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    class is main renderer that renders scene with correct animation and shading.
*/
#ifndef SCENE_RENDERER_H_
#define SCENE_RENDERER_H_

#include "ShaderProgram.h"
class PointLight;
class SpotLight;
class DirectionalLight;
class Object;
class Scene;
class Camera;
class Material;

class SceneRenderer
{
public:
    void Render(std::shared_ptr<Scene> pScene);
    void SetRendererProgram(ShaderProgram program);
    ShaderProgram m_program;

private:
    void SendPointLight(Camera& camera, int index, PointLight& light);
    void SendSpotLight(Camera& camera, SpotLight& light);
    void SendDirectionalLight(Camera& camera, int index, DirectionalLight& light);
    void SendMatrices(Camera& camera, Object& object);
    void RenderObject(Camera& camera, Object& object);
    void SendMaterial(Material& material);
};

#endif

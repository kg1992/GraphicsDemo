/*
    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    class is main renderer that renders scene with correct animation and shading.
*/
#ifndef SCENE_RENDERER_H_
#define SCENE_RENDERER_H_

#include "ShaderProgram.h"
class PointLight;
class SpotLight;
class Object;
class Scene;
class Camera;

class SceneRenderer
{
public:
    void Render(std::shared_ptr<Scene> pScene);
    ShaderProgram m_program;

private:
    void SendPointLight(Camera& camera, ShaderProgram& program, int index, PointLight& light);
    void SendSpotLight(Camera& camera, ShaderProgram& program, SpotLight& light);
    void SendMatrices(Camera& camera, ShaderProgram& program, Object& object);
    void RenderObject(Camera& camera, ShaderProgram& program, Object& object);
};

#endif

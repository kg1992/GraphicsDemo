/*
    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    GizmoRenderer class definition.
    The class helps visualize entities that has no mesh ( such as lights, camera )
*/
#ifndef GIZMO_RENDERER_H_
#define GIZMO_RENDERER_H_

class Scene;
class Object;
class ShaderProgram;
class Camera;

class GizmoRenderer
{
public:
    void Render(std::shared_ptr<Scene> pScene);

private:
    float m_clientWidth;
    float m_clientHeight;

    void SendMatrices(Camera& camera, ShaderProgram& program, Object& object);
    void DrawPointLights(std::shared_ptr<Scene> pScene);
    void DrawSpotLight(std::shared_ptr<Scene> pScene);
    void DrawObjectCenter(std::shared_ptr<Scene> pScene);
};

#endif

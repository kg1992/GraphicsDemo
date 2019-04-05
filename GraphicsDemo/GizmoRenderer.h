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
class PerspectiveCamera;

class GizmoRenderer
{
public:
    void Render(PerspectiveCamera& camera, std::shared_ptr<Scene> pScene);

private:
    float m_clientWidth;
    float m_clientHeight;

    void SendMatrices(PerspectiveCamera& camera, ShaderProgram& program, Object& object);
    void DrawPointLights(PerspectiveCamera& camera, std::shared_ptr<Scene> pScene);
    void DrawSpotLight(PerspectiveCamera& camera, std::shared_ptr<Scene> pScene);
    void DrawObjectCenter(PerspectiveCamera& camera, std::shared_ptr<Scene> pScene);
    void DrawPerspectiveCamera(PerspectiveCamera& camera, std::shared_ptr<Scene> pScene);
};

#endif

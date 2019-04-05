/*
    PeekViewportRenderer.h

    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    The class helps visualize attributes by drawing scene with colorized attribute on the bottom of the screen.
*/
#ifndef PEEK_VIEWPORT_RENDERER_H_
#define PEEK_VIEWPORT_RENDERER_H_

class PerspectiveCamera;
class ShaderProgram;
class Object;
class Scene;

class PeekViewportRenderer
{
public:
    void SetClientRect(int width, int height)
    {
        m_clientWidth = width;
        m_clientHeight = height;
    }

    void Render(std::shared_ptr<Scene> pScene);

private:
    int m_clientWidth;
    int m_clientHeight;

    void RenderObject(PerspectiveCamera& camera, ShaderProgram& program, Object& object);
    void SendMatrices(PerspectiveCamera& camera, ShaderProgram& program, Object& object);
};

#endif
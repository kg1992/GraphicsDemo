#ifndef SKYBOX_RENDERER_H_
#define SKYBOX_RENDERER_H_

class Camera;

class SkyboxRenderer
{
public:
    void Render(Camera& camera, GLuint cubeMap);
};

#endif

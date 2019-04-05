#ifndef SKYBOX_RENDERER_H_
#define SKYBOX_RENDERER_H_

class PerspectiveCamera;

class SkyboxRenderer
{
public:
    void Render(PerspectiveCamera& camera, GLuint cubeMap);
};

#endif

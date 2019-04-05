#ifndef SHADOW_MAP_H_
#define SHADOW_MAP_H_

class ShadowMap2D
{
public:
    void Init(GLsizei mapWidth, GLsizei mapHeight, float W, float H, float D);
    void Free();
    GLuint GetFrameBuffer();
    GLuint GetDepthTexture();
    GLsizei GetMapWidth();
    GLsizei GetMapHeight();

private:
    GLuint m_depthTexture;
    GLuint m_fbo;
    GLsizei m_mapWidth;
    GLsizei m_mapHeight;
};

#endif

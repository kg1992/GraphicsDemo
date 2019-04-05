#ifndef SCREEN_BUFFER_H_
#define SCREEN_BUFFER_H_

class ScreenBuffer
{
public:
    void Init(GLsizei screenWidth, GLsizei screenHeight);

    GLuint GetFrameBuffer();

    GLuint GetTexture();

    void Free();

private:
    GLuint m_renderTexture;
    GLuint m_frameBuffer;
    GLuint m_depthBuffer;
};

#endif

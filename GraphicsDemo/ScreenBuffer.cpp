#include "Common.h"
#include "ScreenBuffer.h"
#include "Errors.h"

void ScreenBuffer::Init(GLsizei screenWidth, GLsizei screenHeight)
{
    glGenFramebuffers(1, &m_frameBuffer);
    GET_AND_HANDLE_GL_ERROR();

    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    GET_AND_HANDLE_GL_ERROR();

    glGenTextures(1, &m_renderTexture);
    GET_AND_HANDLE_GL_ERROR();
    glBindTexture(GL_TEXTURE_2D, m_renderTexture);
    GET_AND_HANDLE_GL_ERROR();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    GET_AND_HANDLE_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GET_AND_HANDLE_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GET_AND_HANDLE_GL_ERROR();

    // Bind the texture to the FBO 
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_renderTexture, 0);
    GET_AND_HANDLE_GL_ERROR();

    // Create the depth buffer 
    glGenRenderbuffers(1, &m_depthBuffer);
    GET_AND_HANDLE_GL_ERROR();
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);
    GET_AND_HANDLE_GL_ERROR();
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
    GET_AND_HANDLE_GL_ERROR();

    // Bind the depth buffer to the FBO 
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer);
    GET_AND_HANDLE_GL_ERROR();

    // Set the target for the fragment shader outputs 
    GLenum drawBufs[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBufs);
    GET_AND_HANDLE_GL_ERROR();
}

GLuint ScreenBuffer::GetFrameBuffer()
{
    return m_frameBuffer;
}

GLuint ScreenBuffer::GetTexture()
{
    return m_renderTexture;
}

void ScreenBuffer::Free()
{
    glDeleteTextures(1, &m_renderTexture);
    glDeleteRenderbuffers(1, &m_depthBuffer);
    glDeleteFramebuffers(1, &m_frameBuffer);
}

#include "Common.h"
#include "Errors.h"
#include "ShadowMap.h"

void ShadowMap2D::Init(GLsizei mapWidth, GLsizei mapHeight, float W, float H, float D)
{
    glGenTextures(1, &m_depthTexture);
    GET_AND_HANDLE_GL_ERROR();
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    GET_AND_HANDLE_GL_ERROR();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, mapWidth, mapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    GET_AND_HANDLE_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    GET_AND_HANDLE_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GET_AND_HANDLE_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    GET_AND_HANDLE_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GET_AND_HANDLE_GL_ERROR();
    GLfloat border[] = { 1.0f, 0.0f, 0.0f, 0.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
    GET_AND_HANDLE_GL_ERROR();

    glBindTexture(GL_TEXTURE_2D, m_depthTexture);

    glGenFramebuffers(1, &m_fbo);
    GET_AND_HANDLE_GL_ERROR();
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    GET_AND_HANDLE_GL_ERROR();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);
    GET_AND_HANDLE_GL_ERROR();
    GLenum drawBuffers[] = { GL_NONE };
    glDrawBuffers(1, drawBuffers);
    GET_AND_HANDLE_GL_ERROR();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_mapWidth = mapWidth;
    m_mapHeight = mapHeight;
}

void ShadowMap2D::Free()
{
    if (m_depthTexture)
    {
        glDeleteTextures(1, &m_depthTexture);
        m_depthTexture = 0;
    }
    if (m_fbo)
    {
        glDeleteFramebuffers(1, &m_fbo);
        m_fbo = 0;
    }
    m_mapWidth = 0;
    m_mapHeight = 0;
}

GLuint ShadowMap2D::GetFrameBuffer()
{
    return m_fbo;
}

GLuint ShadowMap2D::GetDepthTexture()
{
    return m_depthTexture;
}

GLsizei ShadowMap2D::GetMapWidth()
{
    return m_mapWidth;
}

GLsizei ShadowMap2D::GetMapHeight()
{
    return m_mapHeight;
}

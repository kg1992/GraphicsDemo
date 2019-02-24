#ifndef ERRORS_H_
#define ERRORS_H_

#include <string>
#include <vector>
#include <Windows.h>
#include "GL/glad.h"

#define EXIT_ON_ERROR

#define GET_AND_HANDLE_GL_ERROR() GetAndHandleGLError(__FILE__,__LINE__)

void GetAndHandleGLError(const char *file, int line);

void HandleError(const std::string& message);

void HandleError(const char* const message);

void HandleError(const std::wstring& message);

void HandleWinapiError(const std::wstring& message);

void APIENTRY OpenglCallbackFunction(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam);

#endif

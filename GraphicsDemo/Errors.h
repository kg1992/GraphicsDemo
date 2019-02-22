#ifndef ERRORS_H_
#define ERRORS_H_

#include <string>
#include <Windows.h>
#include <vector>
#include "GL/glew.h"
#include "gl/GL.h"

#define EXIT_ON_ERROR

void HandleError(const std::string& message);

void HandleError(const char* const message);

void HandleGLError(const std::string& message, GLenum errCode);

void HandleError(const std::wstring& message);

void HandleWinapiError(const std::wstring& message);

#endif

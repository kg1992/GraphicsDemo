/*
    Errors.cpp

    Error Handling Code.
*/
#include <string>
#include <vector>
#include <iostream>
#include <Windows.h>
#include "Errors.h"

void HandleError(const std::string& message)
{
    HandleError(std::wstring(message.cbegin(), message.cend()));
}

void HandleError(const char* const message)
{
    HandleError(std::string(message));
}

void GetAndHandleGLError(const char *file, int line) {
    GLenum err = glGetError();

    while (err != GL_NO_ERROR) {
        std::string error;

        switch (err) {
        case GL_INVALID_OPERATION:      error = "INVALID_OPERATION";      break;
        case GL_INVALID_ENUM:           error = "INVALID_ENUM";           break;
        case GL_INVALID_VALUE:          error = "INVALID_VALUE";          break;
        case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY";          break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
        }

        std::cerr << "GL_" << error.c_str() << " - " << file << ":" << line << std::endl;
        err = glGetError();
        DebugBreak();
    }
}

void HandleError(const std::wstring& message)
{
    MessageBox(NULL, message.c_str(), TEXT("System"), MB_OK);
#ifdef EXIT_ON_ERROR
    exit(0);
#endif
}

// https://docs.microsoft.com/en-us/windows/desktop/debug/system-error-codes--0-499-
// https://stackoverflow.com/questions/1387064/how-to-get-the-error-message-from-the-error-code-returned-by-getlasterror
void HandleWinapiError(const std::wstring& message)
{
    DWORD dwLastError = GetLastError();
    LPTSTR messageBuffer = nullptr;
    size_t size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, dwLastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&messageBuffer, 0, NULL);
    std::wstring errorMessage = message + L" Code : " + std::to_wstring(dwLastError) + L"(" + messageBuffer + L")";

    MessageBox(NULL, errorMessage.c_str(), TEXT("WINAPI"), MB_OK);

#ifdef EXIT_ON_ERROR
    exit(0);
#endif
}

// https://blog.nobel-joergensen.com/2013/02/17/debugging-opengl-part-2-using-gldebugmessagecallback/
void APIENTRY OpenglCallbackFunction(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam) {
    std::cout << "---------------------opengl-callback-start------------" << std::endl;
    std::cout << "message: " << message << std::endl;
    std::cout << "type: ";
    switch (type) {
    case GL_DEBUG_TYPE_ERROR: std::cout << "ERROR"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "DEPRECATED_BEHAVIOR"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: std::cout << "UNDEFINED_BEHAVIOR"; break;
    case GL_DEBUG_TYPE_PORTABILITY: std::cout << "PORTABILITY"; break;
    case GL_DEBUG_TYPE_PERFORMANCE: std::cout << "PERFORMANCE"; break;
    case GL_DEBUG_TYPE_OTHER: std::cout << "OTHER"; break;
    }
    std::cout << std::endl;

    std::cout << "id: " << id << std::endl;
    std::cout << "severity: ";
    switch (severity) {
    case GL_DEBUG_SEVERITY_LOW: std::cout << "LOW"; break;
    case GL_DEBUG_SEVERITY_MEDIUM: std::cout << "MEDIUM"; break;
    case GL_DEBUG_SEVERITY_HIGH: std::cout << "HIGH"; break;
    }
    std::cout << std::endl;
    std::cout << "---------------------opengl-callback-end--------------" << std::endl;
}

void InitializeGLDebugging()
{
    if (glDebugMessageCallback) {
        std::cout << "Register OpenGL debug callback " << std::endl;
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(OpenglCallbackFunction, nullptr);
        GLuint unusedIds = 0;
        glDebugMessageControl(GL_DONT_CARE,
            GL_DONT_CARE,
            GL_DONT_CARE,
            0,
            &unusedIds,
            true);
    }
    else
        std::cout << "glDebugMessageCallback not available" << std::endl;
}
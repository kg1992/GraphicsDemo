#include <string>
#include <Windows.h>
#include <vector>
#include "GL/glew.h"
#include "gl/GL.h"
#include "Errors.h"

void HandleError(const std::string& message)
{
    HandleError(std::wstring(message.cbegin(), message.cend()));
}

void HandleError(const char* const message)
{
    HandleError(std::string(message));
}

void HandleGLError(const std::string& message, GLenum errCode)
{
    std::vector<char> buffer(256, '\0');
    sprintf(buffer.data(), "%s ( error code : %i(%s) )", message.c_str(), errCode, glewGetErrorString(errCode));
    HandleError(buffer.data());
}

void HandleError(const std::wstring& message)
{
#ifdef EXIT_ON_ERROR
    MessageBox(NULL, message.c_str(), TEXT("System"), MB_OK);
    exit(0);
#endif
}

// https://docs.microsoft.com/en-us/windows/desktop/debug/system-error-codes--0-499-
// https://stackoverflow.com/questions/1387064/how-to-get-the-error-message-from-the-error-code-returned-by-getlasterror
void HandleWinapiError(const std::wstring& message)
{
#ifdef EXIT_ON_ERROR
    DWORD dwLastError = GetLastError();
    LPTSTR messageBuffer = nullptr;
    size_t size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, dwLastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&messageBuffer, 0, NULL);
    std::wstring errorMessage = message + L" Code : " + std::to_wstring(dwLastError) + L"(" + messageBuffer + L")";

    MessageBox(NULL, errorMessage.c_str(), TEXT("WINAPI"), MB_OK);
    exit(0);
#endif
}


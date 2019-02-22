#ifndef ERRORS_H_
#define ERRORS_H_

#include <string>
#include <Windows.h>

#define EXIT_ON_ERROR

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

#endif

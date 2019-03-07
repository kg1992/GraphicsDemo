/*
    Common.h

    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    References : 
     - _DEBUG vs NDEBUG
     https://stackoverflow.com/questions/2290509/debug-vs-ndebug
     - Character Set
     https://stackoverflow.com/questions/18959551/what-is-character-set-in-visual-studio

    Common headers and macro definitions for GraphicsDemo project.

    Todo:
        - Turn this into precompiled header
*/
#ifndef NDEBUG
#   define GD_USE_CONSOLE
#endif

// ref : 
#ifdef _UNICODE
#   define GD_TSTRING std::wstring
#elif _MBCS
#   define GD_TSTRING std::string
#endif
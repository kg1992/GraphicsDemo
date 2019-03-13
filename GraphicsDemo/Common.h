/*
    Common.h

    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    References : 
     - _DEBUG vs NDEBUG
     https://stackoverflow.com/questions/2290509/debug-vs-ndebug
     - Character Set
     https://stackoverflow.com/questions/18959551/what-is-character-set-in-visual-studio

    Common headers and macro definitions for GraphicsDemo project.

    Changelog:
        190307 - Now Common.h is precompiled header
*/
#ifndef NDEBUG
#   define GD_USE_CONSOLE
#endif

#ifdef _UNICODE
#   define GD_TSTRING std::wstring
#elif _MBCS
#   define GD_TSTRING std::string
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <algorithm>
#include <vector>
#include <cstdint>
#include <climits>
#include <string>
#include <map>
#include <unordered_map>
#include <functional>
#include <queue>
#include <stack>
#include <iomanip>

#include <Windows.h>
#include <Windowsx.h>
#include <WinUser.h>

#include <glad.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform.hpp>

#include <fbxsdk.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H 



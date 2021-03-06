/*
    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    ShaderPrograms class definition.
*/
#ifndef SHADER_PROGRAMS_H_
#define SHADER_PROGRAMS_H_

#include "ShaderProgram.h"

class ShaderPrograms
{
public:
    static ShaderProgram s_position;
    static ShaderProgram s_uv;
    static ShaderProgram s_normal;
    static ShaderProgram s_plane;
    static ShaderProgram s_pointLight;
    static ShaderProgram s_phong;
    static ShaderProgram s_axes;
    static ShaderProgram s_normalMap;
    static ShaderProgram s_skybox;
    static ShaderProgram s_screen;
    static ShaderProgram s_texture;

    static void Init();
};

#endif

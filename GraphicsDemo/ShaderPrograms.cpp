/*
    ShaderPrograms.cpp

    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    ShaderPrograms class definition
*/
#include "ShaderPrograms.h"

void ShaderPrograms::Init()
{
    s_basic.Init("Basic", "basic.vert.glsl", "basic.frag.glsl");
    s_position.Init("Position", "basic.vert.glsl", "position.frag.glsl");
    s_uv.Init("UV", "basic.vert.glsl", "uv.frag.glsl");
    s_normal.Init("Normal", "basic.vert.glsl", "normal.frag.glsl");
    s_plane.Init("Plane", "billboard.vert.glsl", "uv.frag.glsl");
    s_pointLight.Init("Billboard", "billboard.vert.glsl", "circle.frag.glsl");
    s_phong.Init("Phong", "phong.vert.glsl", "phong.frag.glsl");
    s_axes.Init("Axes", "axes.vert.glsl", "color.frag.glsl");
}

ShaderProgram ShaderPrograms::s_basic;
ShaderProgram ShaderPrograms::s_position;
ShaderProgram ShaderPrograms::s_uv;
ShaderProgram ShaderPrograms::s_normal;
ShaderProgram ShaderPrograms::s_plane;
ShaderProgram ShaderPrograms::s_pointLight;
ShaderProgram ShaderPrograms::s_phong;
ShaderProgram ShaderPrograms::s_axes;
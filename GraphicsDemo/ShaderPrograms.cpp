/*
    ShaderPrograms.cpp

    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    ShaderPrograms class definition
*/
#include "Common.h"
#include "ShaderPrograms.h"

namespace
{
    const std::string ShaderFileLocation = "./Resources/Shader/";
    const std::string ResolveShaderFilename(const char* const filename)
    {
        return ShaderFileLocation + filename;
    }
}

void ShaderPrograms::Init()
{
    s_position.Init("Position"
        , ResolveShaderFilename("basic.vert.glsl")
        , ResolveShaderFilename("position.frag.glsl"));
    s_uv.Init("UV"
        , ResolveShaderFilename("basic.vert.glsl")
        , ResolveShaderFilename("uv.frag.glsl"));
    s_normal.Init("Normal"
        , ResolveShaderFilename("basic.vert.glsl")
        , ResolveShaderFilename("normal.frag.glsl"));
    s_plane.Init("Plane"
        , ResolveShaderFilename("billboard.vert.glsl")
        , ResolveShaderFilename("uv.frag.glsl"));
    s_pointLight.Init("Billboard"
        , ResolveShaderFilename("billboard.vert.glsl")
        , ResolveShaderFilename("circle.frag.glsl"));
    s_phong.Init("Phong"
        , ResolveShaderFilename("phong.vert.glsl")
        , ResolveShaderFilename("phong.frag.glsl"));
    s_axes.Init("Axes"
        , ResolveShaderFilename("axes.vert.glsl")
        , ResolveShaderFilename("color.frag.glsl"));
    s_normalMap.Init("NormalMap"
        , ResolveShaderFilename("basic.vert.glsl")
        , ResolveShaderFilename("normalMap.frag.glsl"));
    s_skybox.Init("Skybox"
        , ResolveShaderFilename("skybox.vert.glsl")
        , ResolveShaderFilename("skybox.frag.glsl"));
    s_screen.Init("Screen"
        , ResolveShaderFilename("screen.vert.glsl")
        , ResolveShaderFilename("screen.frag.glsl"));
    s_texture.Init("Texture"
        , ResolveShaderFilename("screen.vert.glsl")
        , ResolveShaderFilename("texture.frag.glsl"));
}

ShaderProgram ShaderPrograms::s_position;
ShaderProgram ShaderPrograms::s_uv;
ShaderProgram ShaderPrograms::s_normal;
ShaderProgram ShaderPrograms::s_plane;
ShaderProgram ShaderPrograms::s_pointLight;
ShaderProgram ShaderPrograms::s_phong;
ShaderProgram ShaderPrograms::s_axes;
ShaderProgram ShaderPrograms::s_normalMap;
ShaderProgram ShaderPrograms::s_skybox;
ShaderProgram ShaderPrograms::s_screen;
ShaderProgram ShaderPrograms::s_texture;
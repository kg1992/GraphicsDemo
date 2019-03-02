#ifndef SHADER_PROGRAMS_H_
#define SHADER_PROGRAMS_H_

#include <glad.h>

class ShaderPrograms
{
public:
    static GLuint s_basic;
    static GLuint s_position;
    static GLuint s_uv;
    static GLuint s_normal;

    static void Init();
};

#endif

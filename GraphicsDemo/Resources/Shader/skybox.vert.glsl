#version 430 core

out vec3 vPosition;

uniform mat4 vpMatrix;

const vec3 Cube[] =
{
vec3(-1.f,-1.f,-1.f),
    vec3(-1.f,-1.f, 1.f),
    vec3(-1.f, 1.f, 1.f),
    vec3(1.f, 1.f,-1.f),
    vec3(-1.f,-1.f,-1.f),
    vec3(-1.f, 1.f,-1.f),
    vec3(1.f,-1.f, 1.f),
    vec3(-1.f,-1.f,-1.f),
    vec3(1.f,-1.f,-1.f),
    vec3(1.f, 1.f,-1.f),
    vec3(1.f,-1.f,-1.f),
    vec3(-1.f,-1.f,-1.f),
    vec3(-1.f,-1.f,-1.f),
    vec3(-1.f, 1.f, 1.f),
    vec3(-1.f, 1.f,-1.f),
    vec3(1.f,-1.f, 1.f),
    vec3(-1.f,-1.f, 1.f),
    vec3(-1.f,-1.f,-1.f),
    vec3(-1.f, 1.f, 1.f),
    vec3(-1.f,-1.f, 1.f),
    vec3(1.f,-1.f, 1.f),
    vec3(1.f, 1.f, 1.f),
    vec3(1.f,-1.f,-1.f),
    vec3(1.f, 1.f,-1.f),
    vec3(1.f,-1.f,-1.f),
    vec3(1.f, 1.f, 1.f),
    vec3(1.f,-1.f, 1.f),
    vec3(1.f, 1.f, 1.f),
    vec3(1.f, 1.f,-1.f),
    vec3(-1.f, 1.f,-1.f),
    vec3(1.f, 1.f, 1.f),
    vec3(-1.f, 1.f,-1.f),
    vec3(-1.f, 1.f, 1.f),
    vec3(1.f, 1.f, 1.f),
    vec3(-1.f, 1.f, 1.f),
    vec3(1.f,-1.f, 1.f)
};

void main(void)
{
	vPosition = Cube[gl_VertexID];
	
	gl_Position = vpMatrix * vec4(Cube[gl_VertexID], 1.0);
}

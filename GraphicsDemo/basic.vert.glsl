#version 430 core

layout (location=0) in vec4 position;
layout (location=1) in vec2 uv;
layout (location=2) in vec3 normal;

out vec2 vUv;
out vec3 vPosition;
out vec3 vNormal;

uniform mat4 mvMatrix;
uniform mat4 projMatrix;
uniform mat4 eyeMatrix;

void main(void)
{
	gl_Position = projMatrix * eyeMatrix * mvMatrix * position;

	vUv = uv;

	vPosition = position.xyz;

	vNormal = normal;
}

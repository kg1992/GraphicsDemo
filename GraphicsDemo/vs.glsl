#version 430 core

in vec4 position;
in vec2 uv;
out vec2 vUv;
out vec3 vPosition;

uniform mat4 mvMatrix;
uniform mat4 projMatrix;
uniform mat4 eyeMatrix;

void main(void)
{
	// gl_Position = position;
	// gl_Position = projMatrix * position;
	gl_Position = projMatrix * eyeMatrix * mvMatrix * position;
	vUv = uv;
	vPosition = position.xyz;
}

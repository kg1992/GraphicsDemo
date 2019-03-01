#version 430 core

in vec4 position;

out vec2 v_uv;

uniform mat4 mvMatrix;
uniform mat4 projMatrix;
uniform mat4 eyeMatrix;

void main(void)
{
	// gl_Position = position;
	// gl_Position = projMatrix * position;
	gl_Position = projMatrix * eyeMatrix * mvMatrix * position;
	v_uv = position.xy;
}

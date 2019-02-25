#version 430 core

in vec4 position;

out VS_OUT
{
	vec4 color;
} vs_out;

uniform mat4 mvMatrix;
uniform mat4 projMatrix;
uniform mat4 eyeMatrix;

void main(void)
{
	// gl_Position = position;
	// gl_Position = projMatrix * position;
	gl_Position = projMatrix * eyeMatrix * mvMatrix * position;
	vs_out.color = position * 2.0 + vec4(0.5,0.5,0.5,0.0);
}

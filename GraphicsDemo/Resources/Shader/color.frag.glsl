/*
	color.frag.glsl
	
	this shader puts input color 'vColor' directly into FragColor
*/
#version 430 core

in vec3 vColor;

out vec4 FragColor;

void main(void)
{
	FragColor = vec4(vColor, 1);
}

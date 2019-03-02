#version 430 core

in vec3 vNormal;

out vec4 color;

void main(void)
{
	color = vec4( vNormal, 1 );
}

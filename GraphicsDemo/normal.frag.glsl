#version 430 core

in vec3 vWorldNormal;

out vec4 color;

void main(void)
{
	color = vec4( vWorldNormal, 1 );
}

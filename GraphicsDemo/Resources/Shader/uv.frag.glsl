#version 430 core

in vec2 vUv;
in vec3 vPosition;

out vec4 color;

void main(void)
{
	color = vec4( vUv.xy, 0, 1 );
}

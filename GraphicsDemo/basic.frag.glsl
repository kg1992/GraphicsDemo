#version 430 core

uniform sampler2D s;

in vec2 vUv;
in vec3 vPosition;

out vec4 color;

void main(void)
{
	color = texture( s, vec2(vUv.x, vUv.y) );
}

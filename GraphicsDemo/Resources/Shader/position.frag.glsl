#version 430 core

in vec2 vUv;
in vec3 vPosition;

out vec4 color;

void main(void)
{
	float maxVal = max(max(vPosition.x, vPosition.y), vPosition.z);
	color = vec4( vPosition / maxVal, 1 );
}

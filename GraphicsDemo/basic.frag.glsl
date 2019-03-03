#version 430 core

uniform sampler2D s;

uniform vec3 lightPosition;

in vec2 vUv;
in vec3 vPosition;
in vec3 vLightIntensity;

out vec4 color;

void main(void)
{
	vec4 diffuse = texture( s, vec2(vUv.x, vUv.y) );

	color = diffuse * vec4(vLightIntensity, 1);
}

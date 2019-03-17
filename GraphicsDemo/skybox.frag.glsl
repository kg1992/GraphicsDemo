#version 430 core

in vec3 vPosition;

out vec4 FragColor;

uniform samplerCube uSkyboxMap;

void main(void)
{
	vec4 color = texture(uSkyboxMap, normalize(vPosition));
	FragColor = color;
}
/*
	basic.vert.glsl
	
	basic vertex shader used for rendering most of the objects in GraphicsDemo project.

	Reference :
		https://github.com/PacktPublishing/OpenGL-4-Shading-Language-Cookbook-Third-Edition
*/
#version 430 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in ivec3 bones;
layout (location = 4) in vec3 weights;
layout (location = 5) in vec4 tangent;

out vec2 vUv;
out vec3 vPosition;
out vec3 vWorldNormal;

uniform mat4 mwMatrix;
uniform mat4 mvMatrix;
uniform mat3 normalMatrix;
uniform mat4 projMatrix;

const int LightCount = 1;

uniform struct LightInfo
{
	vec4 position;
	vec3 la;
	vec3 l;
} light[LightCount];

void main(void)
{
	vPosition = position.xyz;

	vWorldNormal = normalize(mwMatrix * vec4(normal, 0)).xyz;

	vUv = uv;
	
	gl_Position = projMatrix * mvMatrix * position;
}

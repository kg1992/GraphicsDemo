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

out vec2 vUv;
out vec3 vPosition;
out vec3 vWorldNormal;

uniform mat4 mwMatrix;
uniform mat4 mvMatrix;
uniform mat4 projMatrix;

void main(void)
{
	vPosition = position.xyz;

	vWorldNormal = normalize(mwMatrix * vec4(normal, 0)).xyz;

	vUv = uv;
	
	gl_Position = projMatrix * mvMatrix * position;
}

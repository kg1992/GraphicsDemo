/*
	phong.vert.glsl
	
	phong vertex shader used for rendering scene in GraphicsDemo project.

	Reference :
		https://github.com/PacktPublishing/OpenGL-4-Shading-Language-Cookbook-Third-Edition
*/
#version 430 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;

out vec2 vUv;
out vec3 vEyePosition;
out vec3 vNormal;

uniform mat4 mwMatrix;
uniform mat4 mvMatrix;
uniform mat3 normalMatrix;
uniform mat4 projMatrix;

void main(void)
{
	vEyePosition = vec3(mvMatrix * position);
	vNormal = normalize(normalMatrix * normal);
	vUv = uv;	
	gl_Position = projMatrix * mvMatrix * position;
}

/*
	basic.frag.glsl
	
	basic fragment shader used for rendering most of the objects in GraphicsDemo project.

	Reference :
		https://github.com/PacktPublishing/OpenGL-4-Shading-Language-Cookbook-Third-Edition
*/
#version 430 core

in vec2 vUv;

out vec4 FragColor;

uniform struct MaterialInfo
{
	vec3 ka;
	sampler2D ambientMap;
	vec3 kd;
	sampler2D diffuseMap;
	vec3 ks;
	sampler2D specularMap;
	float shininess;
	sampler2D normalMap;
} material;

void main(void)
{
	vec4 color = texture(material.normalMap , vUv);
	FragColor = color;
}
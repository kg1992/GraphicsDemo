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
layout (location = 3) in ivec3 bones;
layout (location = 4) in vec3 weights;

const int MaximumWeights = 3;
const int MaximumBoneCount = 128;

uniform mat4 jointTransforms[MaximumBoneCount];


out vec2 vUv;
out vec3 vEyePosition;
out vec3 vNormal;

uniform mat4 mwMatrix;
uniform mat4 mvMatrix;
uniform mat3 normalMatrix;
uniform mat4 projMatrix;

void main(void)
{
	vec4 totalLocalPos = vec4(0.0);
	vec4 totalNormal = vec4(0.0);

	for( int i = 0; i < MaximumWeights; ++i )
	{
		mat4 jointTransform = jointTransforms[bones[i]];
		vec4 posePosition = jointTransform * position;
		totalLocalPos += posePosition * weights[i];
		
		vec4 worldNormal = jointTransform * vec4(normal, 0.0);
		totalNormal += worldNormal * weights[i];
	}
	
	vEyePosition = vec3(mvMatrix * totalLocalPos);
	vNormal = normalize(normalMatrix * totalNormal.xyz);
	vUv = uv;
	gl_Position = projMatrix * mvMatrix * totalLocalPos;
}

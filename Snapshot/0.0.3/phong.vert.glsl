/*
	phong.vert.glsl
	
	phong vertex shader used for rendering scene in GraphicsDemo project.

	Reference :
		https://github.com/PacktPublishing/OpenGL-4-Shading-Language-Cookbook-Third-Edition
*/
#version 430 core

const int MaximumWeights = 3;
const int MaximumBoneCount = 128;
const int LightCount = 1;

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in ivec3 bones;
layout (location = 4) in vec3 weights;
layout (location = 5) in vec4 tangent;

uniform struct LightInfo
{
	vec4 position;
	vec3 la;
	vec3 l;
} light[LightCount];

uniform mat4 jointTransforms[MaximumBoneCount];

out vec2 vUv;
out vec3 vEyePosition;
out vec3 vNormal;
out vec3 vTangentLights[LightCount];
out vec3 vTangentViewDir;

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

	// vertex normal in view space
	vec3 viewNormal = normalize(normalMatrix * normal);
	vec3 viewTangent = normalize(normalMatrix * vec3(tangent));
	vec3 viewBitangent = normalize(cross(viewNormal, vec3(viewTangent)) * tangent.w);

	// View space to tangent space transformation
	mat3 vtMatrix = mat3
	(
		viewTangent.x, viewBitangent.x, viewNormal.x,
		viewTangent.y, viewBitangent.y, viewNormal.y,
		viewTangent.z, viewBitangent.z, viewNormal.z
	);

	vec4 viewVertexPosition = mvMatrix * totalLocalPos;

	for( int i = 0; i < LightCount; ++ i)
	{
		vTangentLights[i] = vtMatrix * vec3(light[i].position - viewVertexPosition);
	}
	vTangentViewDir = vtMatrix * -vec3(viewVertexPosition);

	gl_Position = projMatrix * mvMatrix * totalLocalPos;
}

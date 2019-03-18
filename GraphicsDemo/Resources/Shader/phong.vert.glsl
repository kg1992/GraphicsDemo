/*
	phong.vert.glsl
	
	phong vertex shader used for rendering scene in GraphicsDemo project.

	Reference :
		https://github.com/PacktPublishing/OpenGL-4-Shading-Language-Cookbook-Third-Edition
*/
#version 430 core

const int MaximumPointLightCount = 5;
const int ActivePointLightCount = 1;
const int MaximumSpotLightCount = 5;
const int ActiveSpotLightCount = 1;
const int MaximumWeights = 3;
const int MaximumBoneCount = 128;

layout (location = 0) in vec4 aPosition; 	// attribuet vertex position
layout (location = 1) in vec2 aUv;			// attribute vertex uv
layout (location = 2) in vec3 aNormal;		// attribute vertex normal
layout (location = 3) in ivec3 aBones;		// attribute bone indices that affect the vertex
layout (location = 4) in vec3 aWeights;		// attribute bone weights how much the bone affects the vertex
layout (location = 5) in vec4 aTangent;		// attribuet vertex tangent

struct LightInfo
{
	vec4 position;
	vec3 la;
	vec3 l;
};

struct SpotLightInfo {
    vec3 position;  // Position in cam coords
    vec3 l;         // Diffuse/spec intensity
    vec3 la;        // Amb intensity
    vec3 direction; // Direction of the spotlight in cam coords.
    float exponent; // Angular attenuation exponent
    float cutoff;   // Cutoff angle (between 0 and pi/2)
};

uniform LightInfo uLights[MaximumPointLightCount]; // light information

uniform SpotLightInfo uSpot; // spot light information

uniform mat4 uJointTransforms[MaximumBoneCount]; // Bone pose transforms

uniform mat4 uMvMatrix; // modle-view matrix
uniform mat3 uNormalMatrix; // model-view normal matrix
uniform mat4 uVpMatrix; // view-projection matrix
uniform bool uAnimationEnabled; // flag if animation is enabled
uniform bool uNormalMapEnabled; // flag if normal amp is enabled

out vec2 vUv; 			// varying uv
out vec3 vLightDirs[MaximumPointLightCount]; // varying light direction ( can be view-space or tangent-space vector dending on uNormalMapEnabled)
out vec3 vViewDir; // varying tangent view direction ( for light calculation in tangent space)
out vec3 vvNormal; // varying view-space vertex normal

void main(void)
{
	// animation pose applied model space vertex position
	vec4 totalLocalPos = vec4(0.0);
	// animation pose applied model space vertex normal
	vec4 totalNormal = vec4(0.0);

	// apply animation
	if( uAnimationEnabled )
	{
		for( int i = 0; i < MaximumWeights; ++i )
		{
			mat4 jointTransform = uJointTransforms[aBones[i]];
			
			// calculate position
			vec4 posePosition = jointTransform * aPosition;
			totalLocalPos += posePosition * aWeights[i];
			
			// calculate normal
			vec4 worldNormal = jointTransform * vec4(aNormal, 0.0);
			totalNormal += worldNormal * aWeights[i];
		}
	}
	// if animation is not enabled, just use attribute directly
	else
	{
		totalLocalPos = aPosition;
		totalNormal = vec4(aNormal, 0.0);
	}

	// view-spcae normal normalized
	vec3 viewNormal = normalize(uNormalMatrix * aNormal);

	// view-space vertex position
	vec4 vVertexPosition = uMvMatrix * totalLocalPos;

	if( uNormalMapEnabled )
	{
		vec3 viewTangent = normalize(uNormalMatrix * vec3(aTangent));
		vec3 viewBitangent = normalize(cross(viewNormal, viewTangent) * aTangent.w);

		// View-Tangent normal matrix is tangent space matrix transpose.
		mat3 vtMatrix = mat3
		(
			viewTangent.x, viewBitangent.x, viewNormal.x,
			viewTangent.y, viewBitangent.y, viewNormal.y,
			viewTangent.z, viewBitangent.z, viewNormal.z
		);

		// For each light
		for( int i = 0; i < ActivePointLightCount; ++ i)
		{
			// tangent-space light direction
			vLightDirs[i] = vtMatrix * vec3(uLights[i].position - vVertexPosition);
		}
		
		// tangent-space view direction
		vViewDir = vtMatrix * -vec3(vVertexPosition);
	}
	else
	{
		for( int i = 0; i < ActivePointLightCount; ++ i)
		{
			// view-space light direction
			vLightDirs[i] = uNormalMatrix * vec3(uLights[i].position - vVertexPosition);
		}

		// view-space view direction
		vViewDir = uNormalMatrix * -vec3(vVertexPosition);
	}
	
	// output varying vertex uv
	vUv = aUv;

	// output varying view-space vertex normal
	vvNormal = viewNormal;
	
	// output ndc-space vertex position
	gl_Position = uVpMatrix * uMvMatrix * totalLocalPos;
	
}

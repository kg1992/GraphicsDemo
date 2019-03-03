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

out vec3 vFrontColor;

uniform mat4 mwMatrix;
uniform mat4 mvMatrix;
uniform mat3 normalMatrix;
uniform mat4 projMatrix;

const int LightCount = 5;

uniform struct LightInfo
{
	vec4 position;
	vec3 la;
	vec3 l;
} light[LightCount];

uniform struct MaterialInfo
{
	vec3 ka;
	vec3 kd;
	vec3 ks;
	float shininess;
} material;

void getCamSpace( out vec3 norm, out vec3 position ){
	norm = normalize( normalMatrix * normal );
	position = (mvMatrix * vec4(position, 1.0)).xyz;
}

vec3 phongModel( int lightIndex, vec3 position, vec3 n )
{
	vec3 ambient = light[lightIndex].la * material.ka;
	vec3 s = normalize(light[lightIndex].position.xyz - position);
	float sDotN = max( dot(s,n), 0.0);
	vec3 diffuse = material.kd * sDotN;
	vec3 spec = vec3(0.0);
	if( sDotN > 0.0 ){
		vec3 v = normalize(-position.xyz);
		vec3 r = reflect(-s, n);
		spec = material.ks *pow( max( dot(r,v), 0.0 ), material.shininess);
	}
	return ambient + light[lightIndex].l * (diffuse + spec);
}

void main(void)
{
	vPosition = position.xyz;

	vWorldNormal = normalize(mwMatrix * vec4(normal, 0)).xyz;

	vUv = uv;

	vec3 camNorm;
	vec3 camPosition;

	getCamSpace( camNorm, camPosition );

	vFrontColor = vec3(0.0);
	for( int li = 0; li < LightCount; ++li )
	{
		vFrontColor += phongModel( li, camPosition, camNorm );
	}
	
	gl_Position = projMatrix * mvMatrix * position;
}

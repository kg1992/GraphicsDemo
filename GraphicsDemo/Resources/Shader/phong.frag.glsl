/*
	phong.frag.glsl
	
	phong fragment shader used for rendering scene in GraphicsDemo project.

	Reference :
		https://github.com/PacktPublishing/OpenGL-4-Shading-Language-Cookbook-Third-Edition
*/
#version 430 core

const int MaximumPointLightCount = 5;
const int ActivePointLightCount = 1;
const int MaximumSpotLightCount = 5;
const int ActiveSpotLightCount = 1;

in vec2 vUv; // varying uv
in vec3 vLightDirs[MaximumPointLightCount]; // varying light direction ( can be view-space or tangent-space vector dending on uNormalMapEnabled)
in vec3 vViewDir; // varying tangent view direction ( for light calculation in tangent space)
in vec3 vvNormal; // varying view-space vertex normal

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

struct MaterialInfo
{
	vec3 ka;
	sampler2D ambientMap;
	vec3 kd;
	sampler2D diffuseMap;
	vec3 ks;
	sampler2D specularMap;
	float shininess;
	sampler2D normalMap;
};

uniform LightInfo uLights[MaximumPointLightCount];
uniform SpotLightInfo uSpot;
uniform MaterialInfo uMaterial;
uniform bool uNormalMapEnabled; // flag if normal amp is enabled

layout( location = 0 ) out vec4 FragColor;

vec3 blinnPhongNormal( vec3 n, vec3 s, vec3 v);

void main(void)
{
  vec3 norm;
  if( uNormalMapEnabled )
  {
    // tangent-space normal
    norm = texture(uMaterial.normalMap, vUv).xyz;
    norm.xy = 2.0 * norm.xy - 1.0;
    norm = normalize(norm);
  }
  else
  {
    norm = normalize(vvNormal);
  }
    
  // output color
  vec3 s = normalize(vLightDirs[0]);
  vec3 v = normalize(vViewDir);
  vec3 color = blinnPhongNormal(norm, s, v);
  
  // output fragment color
    FragColor = vec4( color, 1.0 );

  
}

vec3 blinnPhongNormal( vec3 n, vec3 s, vec3 v ) {  
  vec3 texColor = texture(uMaterial.diffuseMap, vUv).rgb;

  vec3 ambient = uLights[0].la * texColor;
  float sDotN = max( dot(s,n), 0.0 );
  vec3 diffuse = texColor * sDotN;
  
  vec3 spec = vec3(0.0);
  if( sDotN > 0.0 ) {
    vec3 h = normalize( v + s );
    spec = uMaterial.ks *
           pow( max( dot(h,n), 0.0 ), uMaterial.shininess );
  }
  return ambient + uLights[0].l * (diffuse + spec);
}
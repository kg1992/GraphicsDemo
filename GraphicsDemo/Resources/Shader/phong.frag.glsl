/*
	phong fragment shader used for rendering scene in GraphicsDemo project.

	Reference :
		https://github.com/PacktPublishing/OpenGL-4-Shading-Language-Cookbook-Third-Edition
*/
#version 430 core

const int MaximumPointLightCount = 5;
const int MaximumSpotLightCount = 5;
const int MaximumWeights = 3;
const int MaximumBoneCount = 128;
const int MaximumDirectionalLightCount = 5;

in vec2 vUv; // varying uv
in vec3 vLightDirs[MaximumPointLightCount]; // varying light direction ( can be view-space or tangent-space vector dending on uNormalMapEnabled)
in vec3 vDirectionalLightDirs[MaximumDirectionalLightCount];
in vec3 vViewDir; // varying tangent view direction ( for light calculation in tangent space)
in vec3 vvNormal; // varying view-space vertex normal
in vec4 vShadowCoord[MaximumDirectionalLightCount];

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

struct DirectionalLightInfo {
	vec3 direction;
	vec3 l;
	vec3 la;
  sampler2D shadowMap;
  bool castShadow;
	mat4 shadowMatrix;
};

struct MaterialInfo
{
	vec3 ka;
	sampler2D ambientMap;
  bool useAmbientMap;
	vec3 kd;
	sampler2D diffuseMap;
  bool useDiffuseMap;
	vec3 ks;
	sampler2D specularMap;
  bool useSpecularMap;
	float shininess;
	sampler2D normalMap;
};

// uniform float uTime;
uniform int uActivePointLightCount;
uniform int uActiveSpotLightCount;
uniform int uActiveDirectionalLightCount;
uniform LightInfo uLights[MaximumPointLightCount];
uniform SpotLightInfo uSpot;
uniform DirectionalLightInfo uDirectionalLights[MaximumDirectionalLightCount];
uniform MaterialInfo uMaterial;
uniform bool uNormalMapEnabled; // flag if normal amp is enabled
uniform sampler2D uShadowMap;

layout( location = 0 ) out vec4 FragColor;

vec3 blinnPhongNormal( vec3 n, vec3 s, vec3 v, vec3 la, vec3 l, float shadowFactor );

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
    
  vec3 finalColor = vec3(0.0, 0.0, 0.0);

  for( int i = 0; i < uActivePointLightCount; ++i)
  {
    vec3 s = normalize(vLightDirs[i]);
    vec3 v = normalize(vViewDir);
    vec3 color = blinnPhongNormal(norm, s, v, uLights[i].la, uLights[i].l, 1.0);
    finalColor += color;
  }

  for( int i = 0; i < uActiveDirectionalLightCount; ++i )
  {
    float shadowFactor = 1.0;
    if( uDirectionalLights[i].castShadow )
    {
      vec4 shadowCoord = vShadowCoord[i];
      vec4 shadow = texture(uShadowMap, shadowCoord.xy);
      const float Bias = 0.02;
      if( shadow.z < shadowCoord.z - Bias )
      {
        shadowFactor = 0.1;
      }
    }
    vec3 s = normalize(vDirectionalLightDirs[i]);
    vec3 v = normalize(vViewDir);
    vec3 color = blinnPhongNormal(norm, s, v, uDirectionalLights[i].la, uDirectionalLights[i].l, shadowFactor);
    finalColor += color;
  }

  // Output fragment color
  FragColor = vec4( finalColor, 1.0 );
}

vec3 blinnPhongNormal( vec3 n, vec3 s, vec3 v, vec3 la, vec3 l, float shadowFactor ) {  
  vec3 texColor;
  if(uMaterial.useDiffuseMap)
  {
    texColor = texture(uMaterial.diffuseMap, vUv).rgb;
  }
  else
  {
    texColor = uMaterial.ks;
  }

  vec3 ambient = la * texColor;
  float sDotN = max( dot(s,n), 0.0 );
  vec3 diffuse = texColor * sDotN;
  
  vec3 spec = vec3(0.0);
  if( sDotN > 0.0 ) {
    vec3 h = normalize( v + s );
    spec = uMaterial.ks *
           pow( max( dot(h,n), 0.0 ), uMaterial.shininess );
  }
  return ambient + shadowFactor * l * (diffuse + spec);
}

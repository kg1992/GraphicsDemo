/*
	phong.frag.glsl
	
	phong fragment shader used for rendering scene in GraphicsDemo project.

	Reference :
		https://github.com/PacktPublishing/OpenGL-4-Shading-Language-Cookbook-Third-Edition
*/
#version 430 core

in vec2 vUv;
in vec3 vEyePosition;
in vec3 vNormal;

out vec4 FragColor;

const int LightCount = 1;

uniform struct LightInfo
{
	vec4 position;
	vec3 la;
	vec3 l;
} light[LightCount];

uniform struct MaterialInfo
{
	vec3 ka;
	sampler2D ambientMap;
	vec3 kd;
	sampler2D diffuseMap;
	vec3 ks;
	sampler2D specularMap;
	float shininess;
} material;

vec3 phongModel( int lightIndex, vec3 position, vec3 n );
vec3 blinnPhongModel( int lightIndex, vec3 position, vec3 n );

void main(void)
{
	vec3 color;
	for( int i = 0; i < LightCount; ++i )
	{
		color += blinnPhongModel(i, vEyePosition, normalize(vNormal));
	}
	FragColor = vec4(color,1);
}

/*
 Function : latticeTest
 Parameters:
	Scale - the bigger the number, holes will appear more densly.
	Threshold - 0 < threshold < 1. Lower threshold generates bigger holes.
 Usage : 
	if( latticeTest(scale, threshold) )
	{
		discard;
	}
 Reference : 
	OpenGL 4 Shading Language Cookbook : Discarding fragments to create a perforated look
*/
bool latticeTest(float scale, float threshold)
{
	return all(greaterThan( fract(vUv * scale), vec2(threshold, threshold) ));
}

vec3 phongModel( int lightIndex, vec3 position, vec3 n )
{
	vec3 s;
	if( light[lightIndex].position.w == 0.0 )
	{
		s = normalize(light[lightIndex].position.xyz);
	}
	else
	{
		s = normalize(light[lightIndex].position.xyz - position);
	}
	float sDotN = dot(s, n);

	vec3 ambient = material.ka * light[lightIndex].la;

	vec3 diffuse = material.kd * max(sDotN, 0);

	vec3 spec = vec3(0.0);
	if( sDotN > 0 ){
		vec3 r = reflect(-s, n);
		vec3 v = normalize(-position);
		spec = material.ks * pow(max(dot(r, v), 0), material.shininess);
	}

	//return ambient + (diffuse) * light[lightIndex].l;
	return ambient + (diffuse + spec) * light[lightIndex].l;
}

vec3 blinnPhongModel( int lightIndex, vec3 position, vec3 n )
{
	vec3 s;
	// the light is directional light
	if( light[lightIndex].position.w == 0.0 )
	{
		s = normalize(light[lightIndex].position.xyz);
	}
	// the light is point light
	else
	{
		s = normalize(light[lightIndex].position.xyz - position);
	}

	float sDotN = dot(s, n);
	vec4 ambientMapColor = texture(material.ambientMap, vUv);
	vec3 ambient =  ambientMapColor.rgb + material.ka * light[lightIndex].la;

	vec4 diffuseMapColor = texture(material.diffuseMap, vUv);
	vec3 diffuse = diffuseMapColor.rgb + material.kd * max(sDotN, 0);

	vec4 specularMapColor = texture(material.specularMap, vUv);
	vec3 spec = vec3(0.0);
	if( sDotN > 0 ){
		vec3 v = normalize(-position); 
		vec3 h = normalize(v + s);
		spec = specularMapColor.rgb + material.ks * pow(max(dot(h, n), 0), material.shininess);
	}

	//return ambient + (diffuse) * light[lightIndex].l;
	return ambient + (diffuse + spec) * light[lightIndex].l;
}
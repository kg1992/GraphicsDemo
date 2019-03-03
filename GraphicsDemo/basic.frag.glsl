/*
	basic.frag.glsl
	
	basic fragment shader used for rendering most of the objects in GraphicsDemo project.

	Reference :
		https://github.com/PacktPublishing/OpenGL-4-Shading-Language-Cookbook-Third-Edition
*/
#version 430 core

uniform sampler2D s;

uniform vec3 lightPosition;

in vec2 vUv;
in vec3 vFrontColor;
in vec3 vBackColor;

out vec4 FragColor;

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

void main(void)
{
	vec4 diffuse = texture( s, vec2(vUv.x, vUv.y) );

	const float scale = 100.0;
	const float threshold = 0.2;
	if( latticeTest(scale, threshold) )
	{
		discard;
	}

	if( gl_FrontFacing )
	{
		FragColor = vec4(vFrontColor, 1);
	}
	else
	{
		FragColor = vec4(vBackColor, 1);
	}
}

#version 430 core

uniform sampler2D s;

in vec2 vUv;
in vec3 vPosition;

out vec4 color;

void main(void)
{
	// color = texelFetch(s, ivec2(gl_FragCoord.xy), 0);


	color = texture( s, vec2(vUv.x, vUv.y) );


	//color = vec4(vUv.x, -vUv.y, 0, 1);


	//float maxComp = max(max(vPosition.x, vPosition.y), vPosition.z);
	//vec3 posColor = vPosition / maxComp;
	//color = vec4(posColor, 1);
}

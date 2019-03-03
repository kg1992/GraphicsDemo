#version 430 core

in vec2 vUv;
in vec3 vPosition;

out vec4 color;

void main(void)
{
	vec2 auv = vUv * 2 - 1;

	float distance = length(auv);

	float begin = 0.4;
	float end = 0.3;
	float t = smoothstep(begin, end, distance);
	if( distance > end )
		discard;

	color = vec4( t, t, t, 1.0 );
}

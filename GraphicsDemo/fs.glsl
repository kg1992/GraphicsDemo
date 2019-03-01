#version 430 core

uniform sampler2D s;

in vec2 v_uv;

out vec4 color;

void main(void)
{
	color = texelFetch(s, ivec2(gl_FragCoord.xy), 0);
	// color = texture( s, v_uv );
}

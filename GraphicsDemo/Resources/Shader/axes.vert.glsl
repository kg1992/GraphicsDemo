/*	
	axes vertex shader. Axes information is embeded into the shader. Uses no attribute.

	always draw using :
		glUseArrays(GL_LINES, 0, 6);
*/
#version 430 core

out vec3 vColor;

uniform mat4 mvMatrix;
uniform mat4 projMatrix;

void main(void)
{
	const vec4 axes[] = {
		vec4(0.0, 0.0, 0.0, 1.0),
		vec4(1.0, 0.0, 0.0, 1.0),
		vec4(0.0, 0.0, 0.0, 1.0),
		vec4(0.0, 1.0, 0.0, 1.0),
		vec4(0.0, 0.0, 0.0, 1.0),
		vec4(0.0, 0.0, 1.0, 1.0),
	};

	const vec3 color[] = 
	{
		vec3(1,0,0),
		vec3(1,0,0),
		vec3(0,1,0),
		vec3(0,1,0),
		vec3(0,0,1),
		vec3(0,0,1),
	};

	vColor = color[gl_VertexID];
	
	gl_Position = projMatrix * mvMatrix * axes[gl_VertexID];
}

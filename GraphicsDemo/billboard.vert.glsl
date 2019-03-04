/*
	billboard.vert.glsl

	vertex shader for billboard rendering.

	Plane geometry is embedded. 

	Usage:
		// Draws 100 x 100 size billboard at origin(0,0,0) that is always facing camera
		program.SendUniform("wCenterPos", 0, 0, 0, 1);
		program.SendUniform("wScale", 100.0f, 100.0f);
		program.SendUniform("wvMatrix", 1, false, m_camera.EyeMatrix());
		program.SendUniform("projMatrix", 1, false, m_camera.ProjectionMatrix());

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
*/
#version 430 core

uniform vec4 wCenterPos;
uniform vec2 wScale;
uniform mat4 wvMatrix;
uniform mat4 projMatrix;

uniform vec3 m_eyeLocation;

out vec2 vUv;

void main()
{
	const vec4 plane[] = {
		vec4(-1.0, -1.0, 0, 1.0),
		vec4(1.0, -1.0, 0, 1.0),
		vec4(1.0, 1.0, 0, 1.0),
		vec4(-1.0, 1.0, 0, 1.0)
	};

	vec4 vCenterPos = wvMatrix * wCenterPos;
	vec3 f = normalize( -vCenterPos.xyz );
	vec3 r = normalize( cross( vec3(0, 1, 0), f) );
	vec3 u = cross(f, r);
	mat4 lookMatrix;
	lookMatrix[0] = vec4(r,0);
	lookMatrix[1] = vec4(u,0);
	lookMatrix[2] = vec4(f,0);
	lookMatrix[3] = vCenterPos;
	
	vec4 v = plane[gl_VertexID];
	v.x *= wScale.x;
	v.y *= wScale.y;
	v = lookMatrix * v;
	gl_Position = projMatrix * v;
	vUv = (plane[gl_VertexID].xy + 1) * .5;
}
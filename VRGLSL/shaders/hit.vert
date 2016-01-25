#version 440

uniform mat4 mMVP;

layout(location = 0) in vec3 vVertex;
layout(location = 3) in vec3 vTexture;

out vec3 vVolumeCoordinates;

void main()
{
	vVolumeCoordinates = vTexture;
	gl_Position = mMVP * vec4(vVertex,1.0f);
}
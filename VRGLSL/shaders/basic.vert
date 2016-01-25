#version 440

uniform mat4 mProjection, mModelView;

layout(location = 0) in vec3 vVertex;
layout(location = 3) in vec3 vTexture;

out vec3 first;
//out vec3 vTextureCoord;

void main()
{
	first = vTexture;
	//vTextureCoord = vTexture;
	gl_Position = mProjection * mModelView * vec4(vVertex,1.0f);
}
#version 450 core


uniform int Usetexture;
uniform sampler2D tex;

in vec2 vVertexTexture;
in vec4 vVertexColor;

layout(location = 0) out vec4 vFragColor;

void main(void)
{
	if(Usetexture == 1){
		vFragColor = texture(tex, vVertexTexture) * vVertexColor;
	}else{
		vFragColor = vVertexColor;
	}
}
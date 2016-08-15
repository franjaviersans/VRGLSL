#version 450 core

uniform mat4 mProjection, mModelView;
uniform int Mode;
uniform vec4 vColor1, vColor2, vColor3, vColor4;


layout(location = 0) in vec2 vVertexCoord;
layout(location = 3) in vec2 vTextureCoord;

out vec2 vVertexTexture;
out vec4 vVertexColor;


void main()
{
	if(Mode == 0){
		vVertexColor = vColor1;
	}else if(Mode >= 1){
		if(gl_VertexID == 0) vVertexColor = vColor1;
		if(gl_VertexID == 1) vVertexColor = vColor2;
		if(gl_VertexID == 2) vVertexColor = vColor3;
		if(gl_VertexID == 3) vVertexColor = vColor4; 
		
	}

	vVertexTexture = vTextureCoord;

	if(Mode >= 2){
		if(Mode == 2 && gl_VertexID == 3){
			gl_Position = mProjection * mModelView * vec4(-0.5f,-0.5f,0.0f,1.0f);
		}else if( Mode == 3 && gl_VertexID == 2){
			gl_Position = mProjection * mModelView * vec4(0.5f,-0.5f,0.0f,1.0f);
		}else{
			gl_Position = mProjection * mModelView * vec4(vVertexCoord,0.0f,1.0f);
		}
	}else gl_Position = mProjection * mModelView * vec4(vVertexCoord,0.0f,1.0f);
	
}
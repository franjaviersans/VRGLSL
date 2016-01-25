// $Id: $
//
// Author: Francisco Sans franjaviersans@gmail.com
//
// Complete history on bottom of file

#define FILE_REVISION "$Revision: $"

#include "CubeIntersection.h"
#include "TextureManager.h"
#include <stdlib.h>
#include <iostream>



/**
* Default constructor
*/
CCubeIntersection::CCubeIntersection(GLuint W, GLuint H):m_uiWidth(W), m_uiHeight(H)
{
	//load the shaders
	m_program.loadShader(std::string("shaders/hit.vert"), CGLSLProgram::VERTEX);
	m_program.loadShader(std::string("shaders/hit.frag"), CGLSLProgram::FRAGMENT);
	m_program.create_link();
	m_program.enable();
		m_program.addAttribute("vVertex");
		m_program.addAttribute("vColor");
		m_program.addUniform("mMVP");
	m_program.disable();

	m_iFrameBuffer = -1;
	depthrenderbuffer = -1;

	SetResolution(W,H);
};

/**
* Default destructor
*/
CCubeIntersection::~CCubeIntersection()
{
	glDeleteFramebuffers(1,&m_iFrameBuffer);
	glDeleteFramebuffers(1,&depthrenderbuffer);
}

/**
* Set resolution
*
* @params ResW new window width resolution
* @params ResH new window hegith resolution
*
*/
void CCubeIntersection::SetResolution(GLuint ResW, GLuint ResH)
{
	m_uiWidth = ResW;
	m_uiHeight = ResH;

	//If it exists, unload
	TextureManager::Inst()->UnloadTexture(TEXTURE_BACK_HIT);

	//Create texture!!
	glActiveTexture(GL_TEXTURE0);

	//Create new empty textures
	TextureManager::Inst()->CreateTexture2D(TEXTURE_BACK_HIT, m_uiWidth,m_uiHeight,GL_RGB16F,GL_RGB,GL_FLOAT,GL_NEAREST,GL_NEAREST);


	//Generate Render Buffer
	if(m_iFrameBuffer == -1)
	{
		glGenFramebuffers(1, &m_iFrameBuffer);
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, m_iFrameBuffer);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureManager::Inst()->GetID(TEXTURE_BACK_HIT), 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	// The depth buffer
	if(depthrenderbuffer == -1)
	{
		glGenRenderbuffers(1, &depthrenderbuffer);
	}
	
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, m_uiWidth, m_uiHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

/**
* Method to Draw the Quad
*/
void CCubeIntersection::Draw(glm::mat4 &m_mMVP)
{
	
	//Bind the FrameBuffer to draw
	glBindFramebuffer(GL_FRAMEBUFFER,m_iFrameBuffer);

	//Draw in all the screen
	glViewport(0, 0,  m_uiWidth, m_uiHeight);

	//Set which face to draw
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);

	glCullFace(GL_FRONT);

	glClearColor(0.0f, 0.0f, 0.0f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Draw a Cube
	m_program.enable();
		glUniformMatrix4fv(m_program.getLocation("mMVP"), 1, GL_FALSE, glm::value_ptr(m_mMVP));
		FBOCube::Instance()->Draw();
	m_program.disable();


	//Reset culling
	glCullFace(GL_BACK);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


/**
* Function to use the texture
*/
void CCubeIntersection::Use(GLenum activeTexture)
{
	glActiveTexture(activeTexture);
	TextureManager::Inst()->BindTexture(TEXTURE_BACK_HIT);
}


/**
* Debug Texture
*/
void CCubeIntersection::Debug()
{
	
}

#undef FILE_REVISION

// Revision History:
// $Log: $
// $Header: $
// $Id: $
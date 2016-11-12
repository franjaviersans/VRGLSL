// $Id: $
//
// Author: Francisco Sans franjaviersans@gmail.com
//
// Complete history on bottom of file

#define FILE_REVISION "$Revision: $"

#include "VBOCube.h"

/**
* Class VBOCube.
* Creates a Cube using VBO with the corresponding texture coordinates
*
*					(0,1,1) v7		(1,1,1) v6
*					 *-------------* 
*				   / |			 / |
*	     v3(0,1,0)*-------------*(1,1,0) v2
*				  |v4|(0,0,1)|  |  |
*				  |	 *----------|--* (1,0,1) v5
*				  |	/			| /
*		 v0(0,0,0)*-------------*(1,0,0) v1
*/

/**
* Default constructor
*/
VBOCube::VBOCube()
{

	Init();
}


/**
* Default destructor
*/
VBOCube::~VBOCube()
{

	glDeleteBuffers(1, &m_iVBO);
	glDeleteBuffers(1, &m_iVBOIndex);
	glDeleteVertexArrays(1, &m_iVAO);
}

/**
* Method to Init cube
*/
void VBOCube::Init(){


	GLfloat Vertex[] = {//World					//Color
						-0.5f,-0.5f,0.5f,		0.0f,0.0f,0.0f,	//v0
						0.5f,-0.5f,0.5f,		1.0f,0.0f,0.0f,	//v1
						0.5f,0.5f,0.5f,			1.0f,1.0f,0.0f,	//v2
						-0.5f,0.5f,0.5f,		0.0f,1.0f,0.0f,	//v3
						-0.5f,-0.5f,-0.5f,		0.0f,0.0f,1.0f,	//v4
						0.5f,-0.5f,-0.5f,		1.0f,0.0f,1.0f,	//v5
						0.5f,0.5f,-0.5f,		1.0f,1.0f,1.0f,	//v6
						-0.5f,0.5f,-0.5f,		0.0f,1.0f,1.0f,	//v7
						}; 

	GLuint Indices[] = {0,1,2,0,2,3,	//front
						4,7,6,4,6,5,	//back
						4,0,3,4,3,7,	//left
						1,5,6,1,6,2,	//right
						3,2,6,3,6,7,	//top
						4,5,1,4,1,0,	//bottom
						}; 

	glGenBuffers(1, &m_iVBO);
	glGenBuffers(1, &m_iVBOIndex);

	// bind buffer for positions and copy data into buffer
	// GL_ARRAY_BUFFER is the buffer type we use to feed attributes
	glBindBuffer(GL_ARRAY_BUFFER, m_iVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iVBOIndex);
 
		// feed the buffer, and let OpenGL know that we don't plan to
		// change it (STATIC) and that it will be used for drawing (DRAW)
		glBufferData(GL_ARRAY_BUFFER, 48 * sizeof(GL_FLOAT), Vertex, GL_STATIC_DRAW);

		//Set the index array
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GL_UNSIGNED_INT), Indices, GL_STATIC_DRAW);



	//Generate the VAO
	glGenVertexArrays(1, &m_iVAO);
    glBindVertexArray(m_iVAO );

		// bind buffer for positions and copy data into buffer
		// GL_ARRAY_BUFFER is the buffer type we use to feed attributes
		glBindBuffer(GL_ARRAY_BUFFER, m_iVBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iVBOIndex);

		glEnableVertexAttribArray(WORLD_COORD_LOCATION);
		glVertexAttribPointer(WORLD_COORD_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 6, BUFFER_OFFSET(0)); //Vertex
		glEnableVertexAttribArray(TEXTURE_COORD_LOCATION);
		glVertexAttribPointer(TEXTURE_COORD_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 6, BUFFER_OFFSET(sizeof(GL_FLOAT) * 3)); //Texture	
		
	//Unbind the vertex array	
	glBindVertexArray(0);


	//Disable Buffers and vertex attributes
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/**
* Method to Draw the Cube
*/
void VBOCube::Draw()
{
	
	glBindVertexArray(m_iVAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

}


/**
* Method to Setup rendering cube
*/
void VBOCube::Setup()
{
	glBindVertexArray(m_iVAO);
}

/**
* Method to only draw cube
*/
void VBOCube::OnlyDraw()
{
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

/** 
* Method to Stop rendering cube
*/
void VBOCube::Stop()
{
	glBindVertexArray(0);
}

/**
* Creates an instance of the model
*
* @return an instance of this class
*/
VBOCube & VBOCube::Instance() 
{
	static VBOCube m_cube;	// Guaranteed to be destroyed.
							// Instantiated on first use.
 
   return m_cube;


}

#undef FILE_REVISION

// Revision History:
// $Log: $
// $Header: $
// $Id: $
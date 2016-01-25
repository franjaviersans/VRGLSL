// $Id: $
//
// Author: Francisco Sans franjaviersans@gmail.com
//
// Complete history on bottom of file

#ifndef FBOQuad_H
#define FBOQuad_H

//Includes
#include "Definitions.h"

/**
* Class FBOQuad.
* Creates a Quad using FBO with the corresponding texture coordinates
*
*           (-0.5,0.5)*------*(0.5,0.5)
*					  |		 |
*					  |      |
*          (-0.5,-0.5)*------*(0.5,-0.5)
*/
class FBOQuad
{
	//Functions

	public:
		///Default destructor
		~FBOQuad();

		///Method to Draw the Quad
		void Draw();

		///Method start up Quad rendering
		void StartUp();

		///Method to only Draw the Quad
		void OnlyDraw();

		///Method to stop Quad rendering
		void Stop();

		///Creates an instance of the model
		static FBOQuad* Instance() ;

	private:
		///Default constructor
		FBOQuad();


	//Variables

	private:
		static FBOQuad * m_quad;
		GLuint m_iVBO;
		GLuint m_iVBOIndex;
		GLuint m_iVAO;
};


#endif //FBOQuad_H
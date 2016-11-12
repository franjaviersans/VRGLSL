// $Id: $
//
// Author: Francisco Sans franjaviersans@gmail.com
//
// Complete history on bottom of file

#ifndef VBOCube_H
#define VBOCube_H

//Includes
#include "Definitions.h"


/**
* Class FBOCube.
* Creates a Cube using FBO with the corresponding texture coordinates
*
*					(-.5,.5,-.5) v7		(.5,.5,-.5) v6
*					 *-------------* 
*				   / |		     / |
*	 v3(-.5,.5,.5)*-------------*(.5,.5,.5) v2
*	(-.5,-.5,-.5) |v4|          |  |
*				  |	 *----------|--* (.5,-.5,-.5) v5
*				  |	/	    	| /
*   v0(-.5,-.5,.5)*-------------*(.5,-.5,.5) v1
*/
class VBOCube
{
	//Functions

	public:
		

		///Default destructor
		~VBOCube();

		///Method to Draw the Quad
		void Draw();

		///Method to Setup rendering cube
		void Setup();

		///Method to only draw cube
		void OnlyDraw();

		///Method to Stop rendering cube
		void Stop();

		//Method to Init cube
		void Init();

		///Creates an instance of the model
		static VBOCube & Instance() ;

	private:

		///Default constructor
		VBOCube();

	//Variables

	private:
		GLuint m_iVAO;
		GLuint m_iVBO;
		GLuint m_iVBOIndex;
};


#endif //FBOCube_H
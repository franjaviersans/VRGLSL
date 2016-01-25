// $Id: $
//
// Author: Francisco Sans franjaviersans@gmail.com
//
// Complete history on bottom of file

#ifndef FBOCube_H
#define FBOCube_H

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
class FBOCube
{
	//Functions

	public:
		

		///Default destructor
		~FBOCube();

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
		static FBOCube* Instance() ;

	private:

		///Default constructor
		FBOCube();

	//Variables

	private:
		static FBOCube * m_cube;
		GLuint m_iVAO;
		GLuint m_iVBO;
		GLuint m_iVBOIndex;
};


#endif //FBOCube_H
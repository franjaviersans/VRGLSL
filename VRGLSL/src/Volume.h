// $Id: $
//
// Author: Francisco Sans franjaviersans@gmail.com
//
// Complete history on bottom of file

#ifndef VOLUME_H
#define VOLUME_H

//Includes
#include "Definitions.h"
#include <string>


/**
* Class Volume.
* Class to load a volume from a file and store it in a 3D texture to display.
*/
class Volume
{
	//Functions

	public:

		float diagonal;
		glm::vec3 voxelSize;
		

		///Default constructor
		Volume();

		///Default destructor
		~Volume();

		///
		void Init();

		///
		void Load(std::string , GLuint , GLuint , GLuint, GLboolean, GLuint);

		///
		void Use(GLenum );

		///
		glm::vec3 getVoxelSize();


	//Variables

	private:
};


#endif //FBOCube_H
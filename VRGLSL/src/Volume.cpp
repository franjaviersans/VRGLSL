// $Id: $
//
// Author: Francisco Sans franjaviersans@gmail.com
//
// Complete history on bottom of file

#define FILE_REVISION "$Revision: $"

#include "Volume.h"
#include <fstream>
#include <iostream>
#include "TextureManager.h"

using std::cout;
using std::endl;
using std::string;

/**
* Default constructor
*/
Volume::Volume()
{
	Init();
}


/**
* Default destructor
*/
Volume::~Volume()
{
}


void Volume::Init()
{
	
}

void Volume::Load(string filename, GLuint width, GLuint height, GLuint depth, GLboolean bits, GLuint offset)
{
	//Read texture from file
	std::ifstream textureFile(filename, std::ios::binary);
	
	if(!textureFile.is_open()){
		cout<<"Couldn't load file volume: "<<filename<<endl;
		exit(0);
	}

	int length = -1;
	textureFile.seekg (0, textureFile.end);
	length = int(textureFile.tellg());

	if (length < width * height * depth * ((bits)?sizeof(unsigned char):sizeof(unsigned short))){
		cout << "Bad volume size or wrong file!" << endl;
		exit(0);
	}

    textureFile.seekg (0, textureFile.beg);


	if (bits){
		int file_offset = width * height * offset;
		char * memtexture = new char[length - file_offset];

		textureFile.seekg(file_offset); //Skip some slices
		textureFile.read((char *)memtexture, length - file_offset);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		//Create Texture
		TextureManager::Inst().CreateTexture3D(TEXTURE_VOLUME, width, height, depth, GL_RED, GL_RED, GL_UNSIGNED_BYTE, GL_LINEAR, GL_LINEAR, memtexture);
		delete  [] memtexture;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}
	else{
		int file_offset = width * height * sizeof(unsigned short) * offset;
		short * memtexture = new short[(length - file_offset) / sizeof(unsigned short)];
		
		textureFile.seekg(file_offset); //Skip some slices
		textureFile.read((char *)memtexture, length - file_offset);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
		//Create Texture
		TextureManager::Inst().CreateTexture3D(TEXTURE_VOLUME, width, height, depth, GL_RED, GL_RED, GL_UNSIGNED_SHORT, GL_LINEAR, GL_LINEAR, memtexture);
		delete [] memtexture;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}

	textureFile.close();

	diagonal = sqrtf(float(width * width + height * height + depth * depth));

	voxelSize = glm::vec3(1.0f / width, 1.0f / height, 1.0f / depth);

	GLenum err = GL_NO_ERROR;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::cout << "Problem loading volume " << err << std::endl;
	}
}

void Volume::Use(GLenum activeTexture)
{
	glActiveTexture(activeTexture);
	TextureManager::Inst().BindTexture(TEXTURE_VOLUME);
}

glm::vec3 Volume::getVoxelSize(){
	return voxelSize;
}


#undef FILE_REVISION

// Revision History:
// $Log: $
// $Header: $
// $Id: $
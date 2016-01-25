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

using namespace std;

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

void Volume::Load(string filename, GLuint width, GLuint height, GLuint depth)
{
	//Read texture from file
	std::ifstream textureFile(filename, std::ios::binary);
	
	if(!textureFile.is_open()){
		cout<<"Couldn't load file"<<endl;
		exit(0);
	}

	int length = -1;
	textureFile.seekg (0, textureFile.end);
	length = int(textureFile.tellg());
    textureFile.seekg (0, textureFile.beg);

	char * memtexture = new char[length];
    textureFile.read(memtexture, length);

	textureFile.close();

	diagonal = sqrtf(float(width * width + height * height + depth * depth));

	//std::cout<<width<<"  "<<height<<" "<<depth<<" "<<length<<" "<<width* height* depth<<std::endl;

	//for(int i=0; i<length; ++i) memtexture[i] = 255;

	//Create Texture
	TextureManager::Inst()->CreateTexture3D(TEXTURE_VOLUME, width, height, depth, GL_RED, GL_RED, GL_FLOAT, GL_LINEAR, GL_LINEAR, memtexture);
}

void Volume::Use(GLenum activeTexture)
{
	glActiveTexture(activeTexture);
	TextureManager::Inst()->BindTexture(TEXTURE_VOLUME);
}


#undef FILE_REVISION

// Revision History:
// $Log: $
// $Header: $
// $Id: $
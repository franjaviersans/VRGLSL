//**********************************************
//Singleton Texture Manager class
//Written by Ben English
//benjamin.english@oit.edu
//
//For use with OpenGL and the FreeImage library
//**********************************************

#include "TextureManager.h"
#include <iostream>

TextureManager* TextureManager::m_inst(0);

TextureManager* TextureManager::Inst()
{
	if(!m_inst)
		m_inst = new TextureManager();

	return m_inst;
}

TextureManager::TextureManager()
{
	// call this ONLY when linking with FreeImage as a static library
	#ifdef FREEIMAGE_LIB
		FreeImage_Initialise();
	#endif
}

//these should never be called
//TextureManager::TextureManager(const TextureManager& tm){}
//TextureManager& TextureManager::operator=(const TextureManager& tm){}
	
TextureManager::~TextureManager()
{
	// call this ONLY when linking with FreeImage as a static library
	#ifdef FREEIMAGE_LIB
		FreeImage_DeInitialise();
	#endif

	UnloadAllTextures();
	m_inst = 0;
}

bool TextureManager::LoadTexture2D(const char* filename, const unsigned int texID, GLenum image_format, GLint internal_format, GLint level, GLint border)
{
	//image format
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	//pointer to the image, once loaded
	FIBITMAP *dib(0);
	//pointer to the image data
	BYTE* bits(0);
	//image width and height
	unsigned int width(0), height(0);
	//OpenGL's image ID to map to
	GLuint gl_texID;
	
	//check the file signature and deduce its format
	fif = FreeImage_GetFileType(filename, 0);
	//if still unknown, try to guess the file format from the file extension
	if(fif == FIF_UNKNOWN) 
		fif = FreeImage_GetFIFFromFilename(filename);
	//if still unkown, return failure
	if(fif == FIF_UNKNOWN)
		return false;

	//check that the plugin has reading capabilities and load the file
	if(FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, filename);
	//if the image failed to load, return failure
	if(!dib)
		return false;

	//retrieve the image data
	bits = FreeImage_GetBits(dib);
	//get the image width and height
	width = FreeImage_GetWidth(dib);
	height = FreeImage_GetHeight(dib);
	//if this somehow one of these failed (they shouldn't), return failure
	if((bits == 0) || (width == 0) || (height == 0))
		return false;
	
	//if this texture ID is in use, unload the current texture
	if(m_texID.find(texID) != m_texID.end())
		glDeleteTextures(1, &(m_texID[texID].id));

	//generate an OpenGL texture ID for this texture
	glGenTextures(1, &gl_texID);
	//store the texture ID mapping
	m_texID[texID].id = gl_texID;
	m_texID[texID].type = GL_TEXTURE_2D;
	//bind to the new texture ID
	glBindTexture(GL_TEXTURE_2D, gl_texID);
	//store the texture data for OpenGL use
	glTexImage2D(GL_TEXTURE_2D, level, internal_format, width, height,
		border, image_format, GL_UNSIGNED_BYTE, bits);


	//Free FreeImage's copy of the data
	FreeImage_Unload(dib);

	//return success
	return true;
}

/**
* Create an empty texture
* @params Width the width of the texture
* @params Height the height of the texture
* @params internalFormat the internal format of the texture
* @params pixelFormat the pixel format of the texture
* @params pixelType the pixel type of the texture
* @params minFilter the minification filter of the texture
* @params magFilter the magnification texture of the texture
* @params data the data of te texture (if any is present)	
*
*/
void TextureManager::CreateTexture2D( const unsigned int texID, GLuint Width, GLuint Height, 
								GLint internalFormat, GLenum pixelFormat, GLenum pixelType,
								GLint minFilter, GLint magFilter,
								GLvoid* data)
{	

	//OpenGL's image ID to map to
	GLuint gl_texID;

	//if this texture ID is in use, unload the current texture
	if(m_texID.find(texID) != m_texID.end())
		glDeleteTextures(1, &(m_texID[texID].id));

	//generate an OpenGL texture ID for this texture
	glGenTextures(1, &gl_texID);
	//store the texture ID mapping
	m_texID[texID].id = gl_texID;
	m_texID[texID].type = GL_TEXTURE_2D;
	//bind to the new texture ID
	glBindTexture(GL_TEXTURE_2D, gl_texID);
	//store the texture data for OpenGL use

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, Width,Height, 0, pixelFormat, pixelType, data);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,magFilter);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
}


/**
* Create an empty texture
* @params Width the width of the texture
* @params Height the height of the texture
* @params internalFormat the internal format of the texture
* @params pixelFormat the pixel format of the texture
* @params pixelType the pixel type of the texture
* @params minFilter the minification filter of the texture
* @params magFilter the magnification texture of the texture
* @params data the data of te texture (if any is present)
* @params border[4] the color of the border
*
*/
void TextureManager::CreateEmptyTexture2DClampToBorder( const unsigned int texID, GLuint Width, GLuint Height, 
								GLint internalFormat, GLenum pixelFormat, GLenum pixelType,
								GLint minFilter, GLint magFilter,
								GLvoid* data, GLfloat border[4])
{
	//OpenGL's image ID to map to
	GLuint gl_texID;

	//if this texture ID is in use, unload the current texture
	if(m_texID.find(texID) != m_texID.end())
		glDeleteTextures(1, &(m_texID[texID].id));

	//generate an OpenGL texture ID for this texture
	glGenTextures(1, &gl_texID);
	//store the texture ID mapping
	m_texID[texID].id = gl_texID;
	m_texID[texID].type = GL_TEXTURE_2D;
	//bind to the new texture ID
	glBindTexture(GL_TEXTURE_2D, gl_texID);
	//store the texture data for OpenGL use

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, Width,Height, 0, pixelFormat, pixelType, data);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,magFilter);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
}

///Create an empty texture
void TextureManager::CreateTexture1D( const unsigned int texID, GLuint size, 
							GLint internalFormat, GLenum pixelFormat, GLenum pixelType,
							GLint minFilter, GLint magFilter,
							GLvoid* data)
{
	//OpenGL's image ID to map to
	GLuint gl_texID;

	//if this texture ID is in use, unload the current texture
	if(m_texID.find(texID) != m_texID.end())
		glDeleteTextures(1, &(m_texID[texID].id));

	//generate an OpenGL texture ID for this texture
	glGenTextures(1, &gl_texID);
	//store the texture ID mapping
	m_texID[texID].id = gl_texID;
	m_texID[texID].type = GL_TEXTURE_1D;
	//bind to the new texture ID
	glBindTexture(GL_TEXTURE_1D, gl_texID);
	//store the texture data for OpenGL use

	glTexImage1D(GL_TEXTURE_1D, 0, internalFormat, size, 0, pixelFormat, pixelType, data);
	glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER,magFilter);
	glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER,minFilter);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAX_LEVEL, 0);
}


///Create an empty texture
void TextureManager::CreateTexture3D( const unsigned int texID, GLuint width, GLuint Height, GLuint Depth, 
							GLint internalFormat, GLenum pixelFormat, GLenum pixelType,
							GLint minFilter, GLint magFilter,
							GLvoid* data)
{
	//OpenGL's image ID to map to
	GLuint gl_texID;

	//if this texture ID is in use, unload the current texture
	if(m_texID.find(texID) != m_texID.end())
		glDeleteTextures(1, &(m_texID[texID].id));

	//generate an OpenGL texture ID for this texture
	glGenTextures(1, &gl_texID);
	//store the texture ID mapping
	m_texID[texID].id = gl_texID;
	m_texID[texID].type = GL_TEXTURE_3D;
	//bind to the new texture ID
	glBindTexture(GL_TEXTURE_3D, gl_texID);
	//store the texture data for OpenGL use

	glTexImage3D(GL_TEXTURE_3D, 0, internalFormat, width, Height, Depth, 0, pixelFormat, pixelType, data);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,magFilter);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,minFilter);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 0);
}
	
	

bool TextureManager::UnloadTexture(const unsigned int texID)
{
	bool result(true);
	//if this texture ID mapped, unload it's texture, and remove it from the map
	if(m_texID.find(texID) != m_texID.end())
	{
		glDeleteTextures(1, &(m_texID[texID].id));
		m_texID.erase(texID);
	}
	//otherwise, unload failed
	else
	{
		result = false;
	}

	return result;
}

bool TextureManager::BindTexture(const unsigned int texID)
{
	bool result(true);
	//if this texture ID mapped, bind it's texture as current
	if(m_texID.find(texID) != m_texID.end())
		glBindTexture(m_texID[texID].type, m_texID[texID].id);
	//otherwise, binding failed
	else
		result = false;

	return result;
}

void TextureManager::UnloadAllTextures()
{
	//start at the begginning of the texture map
	std::map<unsigned int, tex>::iterator i = m_texID.begin();

	//Unload the textures untill the end of the texture map is found
	while(i != m_texID.end()){
		glDeleteTextures(1, &(i->second.id));
		++i;
	}
	

	//clear the texture map
	m_texID.clear();
}


///Get OpenGL ID
GLuint TextureManager::GetID(const unsigned int texID){
	//if this texture ID mapped, bind it's texture as current
	if(m_texID.find(texID) != m_texID.end())
		return  m_texID[texID].id;
	//otherwise, binding failed
	else
		return 1000;
}
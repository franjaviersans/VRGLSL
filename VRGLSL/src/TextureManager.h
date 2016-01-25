//**********************************************
//Singleton Texture Manager class
//Written by Ben English
//benjamin.english@oit.edu
//
//For use with OpenGL and the FreeImage library
//**********************************************

#ifndef TextureManager_H
#define TextureManager_H


#include "Definitions.h"
#include "TextureManager.h"
#include <map>

class TextureManager
{
public:
	static TextureManager* Inst();
	virtual ~TextureManager();

	//load a texture an make it the current texture
	//if texID is already in use, it will be unloaded and replaced with this texture
	bool LoadTexture2D(const char* filename,	//where to load the file from
						const unsigned int texID,			//arbitrary id you will reference the texture by
															//does not have to be generated with glGenTextures
						GLenum image_format = GL_RGB,		//format the image is in
						GLint internal_format = GL_RGB,		//format to store the image in
						GLint level = 0,					//mipmapping level
						GLint border = 0);					//border size

	///Create an empty texture 2D
	void CreateTexture2D( const unsigned int texID, GLuint Width = 512, GLuint Height  = 512, 
							GLint internalFormat = GL_RGB, GLenum pixelFormat = GL_RGB, GLenum pixelType = GL_UNSIGNED_BYTE,
							GLint minFilter = GL_NEAREST, GLint magFilter = GL_NEAREST,
							GLvoid* data = 0);

	///Create an empty texture with a border 2D
	void CreateEmptyTexture2DClampToBorder( const unsigned int texID, GLuint Width = 512, GLuint Height  = 512, 
							GLint internalFormat = GL_RGB, GLenum pixelFormat = GL_RGB, GLenum pixelType = GL_UNSIGNED_BYTE,
							GLint minFilter = GL_NEAREST, GLint magFilter = GL_NEAREST,
							GLvoid* data = 0, GLfloat border[4] = NULL);

	///Create an empty texture 1D
	void CreateTexture1D( const unsigned int texID, GLuint size = 512, 
							GLint internalFormat = GL_RGB, GLenum pixelFormat = GL_RGB, GLenum pixelType = GL_UNSIGNED_BYTE,
							GLint minFilter = GL_NEAREST, GLint magFilter = GL_NEAREST,
							GLvoid* data = 0);

	///Create an empty texture 3D
	void CreateTexture3D( const unsigned int texID, GLuint width = 512, GLuint Height  = 512, GLuint Depth  = 512, 
							GLint internalFormat = GL_RGB, GLenum pixelFormat = GL_RGB, GLenum pixelType = GL_UNSIGNED_BYTE,
							GLint minFilter = GL_NEAREST, GLint magFilter = GL_NEAREST,
							GLvoid* data = 0);

	//free the memory for a texture
	bool UnloadTexture(const unsigned int texID);

	//set the current texture
	bool BindTexture(const unsigned int texID);

	//free all texture memory
	void UnloadAllTextures();

	///Get OpenGL ID
	GLuint GetID(const unsigned int texID);

protected:

	struct tex{
		GLuint id;
		GLenum type;
	};

	TextureManager();
	TextureManager(const TextureManager& tm);
	TextureManager& operator=(const TextureManager& tm);

	static TextureManager* m_inst;
	std::map<unsigned int, tex> m_texID;
};

#endif
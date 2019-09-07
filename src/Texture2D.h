#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include "GL/glew.h"

// Texture 2D is able to store and configure a texture in OpenGL.
class Texture2D {
public:
	// Holds the IF of the texture object
	GLuint ID;
	// Texture image dimensions
	GLuint width, height;
	// Texture format
	GLuint internalFormat;
	GLuint imageFormat;
	// Texture configuration
	GLuint wrapS;
	GLuint wrapT;
	GLuint filterMin;
	GLuint filterMax;
	// Constructor
	Texture2D();
	// Generates texture from image data
	void generate(GLuint width, GLuint height, unsigned char* data);
	// Binds the texture as the current active texture
	void bind() const;
};

#endif

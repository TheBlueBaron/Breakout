#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <string>

#include "GL/glew.h"

#include "Texture2D.h"
#include "Shader.h"

/* 
	A static singleton ResourceManager class that hosts several
	functions to load textures and shaders.  Each loaded texture
	and/or shader is also sotred for future reference by string
	handles.  All functions and resoures are static and no public
	constructor is defined.
*/
class ResourceManager {
public:
	// Resource storage
	static std::map<std::string, Shader> shaders;
	static std::map<std::string, Texture2D> textures;
	// Loads (and generates) a shader program from file loading vertex, fragment and geometry shader source code.  If gShaderFile is not null, geometry shader loaded.
	static Shader loadShader(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile, std::string name);
	// Retrieves a stored shader
	static Shader getShader(std::string name);
	// Loads and generates a texture from file
	static Texture2D loadTexture(const GLchar *file, GLboolean alpha, std::string name);
	// Retrieves a stored texture
	static Texture2D getTexture(std::string name);
	// Correctly de-allocate all loaded resources
	static void clear();
private:
	// Private constructor, this is since no ResourceManager objects are wanted, its attributes and functions are publicly available.
	ResourceManager() {}
	// Loads and generates a shader from file
	static Shader loadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile = nullptr);
	// Loads a single texture from file
	static Texture2D loadTextureFromFile(const GLchar *file, GLboolean alpha);
};

#endif // !RESOURCE_MANAGER_H



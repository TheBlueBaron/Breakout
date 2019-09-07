#ifndef SHADER_H
#define SHADER_H

#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


/*
	General purpose shader object.  Complies from a file,
	generates compile/link-time error messages and hosts
	several utility funcitons for easy management.
*/
class Shader {
public:
	// State
	GLuint ID;
	// Consturctor
	Shader() {}
	// Sets the current shader as active
	Shader &use();
	// Compiles the shader from given source code
	void compile(const GLchar *vertexSource, const GLchar *fragmentSource, const GLchar *geometrySource = nullptr); // Geometry source is optional
	// Utility functions
	void setFloat(const GLchar *name, GLfloat value, GLboolean useShader = false);
	void setInteger(const GLchar *name, GLint value, GLboolean useShader = false);
	void setVector2f(const GLchar *name, GLfloat x, GLfloat y, GLboolean useShader = false);
	void setVector2f(const GLchar *name, const glm::vec2 &value, GLboolean useShader = false);
	void setVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader = false);
	void setVector3f(const GLchar *name, const glm::vec3 &value, GLboolean useShader = false);
	void setVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader = false);
	void setVector4f(const GLchar *name, const glm::vec4 &value, GLboolean useShader = false);
	void setMatrix4(const GLchar *name, const glm::mat4 &matrix, GLboolean useShader = false);
private:
	// Checks if compilation or linking has failed and if so, print an error log
	void checkCompileErrors(GLuint object, std::string type);
};

#endif // !SHADER_H


#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include "GL/glew.h"
#include "glm/glm.hpp"

#include "Texture2D.h"
#include "SpriteRenderer.h"
#include "Shader.h"

/*
	PostProcessor holds all post-processing effects for Breakout game.
	It renders the game on a textured quad after which, one can enable
	specific effects.
*/
class PostProcessor {
public:
	// State
	Shader postProcessingShader;
	Texture2D texture;
	GLuint width, height;
	// Options
	GLboolean confuse, chaos, shake;
	// Constructor
	PostProcessor(Shader shader, GLuint width, GLuint height);
	// Prepares the post-processor's framebuffer operations before rendering the game
	void beginRender();
	// Should be called after rendering the game, so all rendered data is stored in a texture object
	void endRender();
	// Renders the PostProcessor text quad
	void render(GLfloat time);
private:
	// Render state
	GLuint MSFBO, FBO; // MSFBO = Multisampled FBO. FBO is regular, used for MS color-buffer to texture
	GLuint RBO; // RBO is used for multisampled color-buffer
	GLuint VAO;
	// Initialise quad for rendering postprocessing texture
	void initRenderData();
};

#endif // !POST_PROCESSOR_H


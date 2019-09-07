#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Texture2D.h"
#include "Shader.h"

class SpriteRenderer {
public:
	// Constructor
	SpriteRenderer(Shader &shader);
	// Destructor
	~SpriteRenderer();
	// Renders a defined quad textured with given sprite
	void drawSprite(Texture2D &texture, glm::vec2 position, glm::vec2 size = glm::vec2(10, 10), GLfloat rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
private:
	// Render state
	Shader shader;
	GLuint quadVAO;
	// Initialise and configures the quad's buffer and vertex attributes
	void initRenderData();
};

#endif // !SPRITE_RENDERER_G

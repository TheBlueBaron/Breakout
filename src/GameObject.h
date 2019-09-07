#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "GL/glew.h"
#include "glm/glm.hpp"

#include "Texture2D.h"
#include "SpriteRenderer.h"

/* 
	Container object for holding all state relevant information for a single
	game object.
*/
class GameObject {
public:
	// Object state
	glm::vec2 position, size, velocity;
	glm::vec3 color;
	GLfloat	rotation;
	GLboolean isSolid;
	GLboolean destroyed;
	// Render state
	Texture2D	sprite;
	// Constructor
	GameObject();
	GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
	// Draw sprite
	virtual void draw(SpriteRenderer &renderer);
};

#endif // !GAMEOBJECT_H

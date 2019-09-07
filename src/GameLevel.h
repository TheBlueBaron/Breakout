#ifndef GAMELEVEL_H
#define GAMELEVEL_H

#include <vector>

#include "GL/glew.h"
#include "glm/glm.hpp"

#include "GameObject.h"
#include "BallObject.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"

// GameLevel holds all tiles that are part of a Breakout level.
class GameLevel {
public:
	// Level state
	std::vector<GameObject> bricks;
	// Constructor
	GameLevel() {}
	// Load level from file
	void load(const GLchar *file, GLuint levelWidth, GLuint levelHeight);
	// Render level
	void draw(SpriteRenderer &renderer);
	// Check if the level is completed (all non-solid tiles are destoryed)
	GLboolean isCompleted();
private:
	// Initialise level from tile data
	void init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight);
};

#endif // !GAMELEVEL_H

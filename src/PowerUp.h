#ifndef POWER_UP_H
#define POWER_UP_H

#include <string>

#include "GL/glew.h"
#include "glm/glm.hpp"

#include "GameObject.h"

// The size of the powerUp block
const glm::vec2 SIZE(60, 20);
// Velocity of powerUp block when spawned
const glm::vec2 VELOCITY(0.0f, 150.0f);

/*
	PowerUp is a subclass of GameObject and contains
	extra information like if it is activated and the duration
	of the powerup
*/
class PowerUp : public GameObject {
public:
	// PowerUp state
	std::string type;
	GLfloat duration;
	GLboolean activated;
	// Constructor
	PowerUp(std::string type, glm::vec3 color, GLfloat duration, glm::vec2 position, Texture2D texture)
		: GameObject(position, SIZE, texture, color, VELOCITY), type(type), duration(duration), activated() {}
};

#endif // !POWER_UP_H


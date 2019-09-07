#ifndef BALLOBJECT_H
#define BALLOBJECT_H

#include "GL/glew.h"
#include "glm/glm.hpp"

#include "Texture2D.h"
//#include "SpriteRenderer.h"
//#include "GameObject.h"
#include "PowerUp.h"

/*
	Ball object is a GameObject with extra properties.
	A radius and if the ball is stuck to the player paddle
*/
class BallObject : public GameObject {
public:
	// Ball state
	GLfloat radius;
	GLboolean stuck;
	GLboolean sticky, passThrough;
	// Constructor
	BallObject();
	BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite);
	// Move the ball, keeping it constrainecd within the window bounds (except bottom edge)
	glm::vec2 move(GLfloat dt, GLuint windowWidth);
	// Reset the ball to original state with given position and velocity
	void reset(glm::vec2 position, glm::vec2 velocity);
};

#endif // !BALLOBJECT_H


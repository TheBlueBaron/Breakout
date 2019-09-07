#include "BallObject.h"

BallObject::BallObject() : GameObject(), radius(12.5f), stuck(true), sticky(GL_FALSE), passThrough(GL_FALSE) {
}

BallObject::BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite)
	: GameObject(pos, glm::vec2(radius * 2, radius *2), sprite, glm::vec3(1.0f), velocity), radius(radius), stuck(true), sticky(GL_FALSE), passThrough(GL_FALSE) {
}

glm::vec2 BallObject::move(GLfloat dt, GLuint windowWidth) {
	// If not stuck to the player paddle
	if (!this->stuck) {
		// Move the ball
		this->position += this->velocity * dt;
		// Then check if ball is out of bounds and if so, reverse velocity and restore at correct position
		if (this->position.x <= 0.0f) {
			this->velocity.x = -this->velocity.x;
			this->position.x = 0.0f;
		}
		else if (this->position.x + this->size.x >= windowWidth) {
			this->velocity.x = -this->velocity.x;
			this->position.x = windowWidth - this->size.x;
		}

		if (this->position.y <= 0.0f) {
			this->velocity.y = -this->velocity.y;
			this->position.y = 0.0f;
		}
	}
	
	return this->position;
}

void BallObject::reset(glm::vec2 position, glm::vec2 velocity) {
	this->position = position;
	this->velocity = velocity;
	this->stuck = GL_TRUE;
	this->sticky = GL_FALSE;
	this->passThrough = GL_FALSE;
}

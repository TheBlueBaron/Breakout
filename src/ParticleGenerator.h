#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H

#include <vector>

#include "GL/glew.h"
#include "glm/glm.hpp"

#include "Shader.h"
#include "Texture2D.h"
#include "GameObject.h"

// Represents a single particle and its state
struct Particle {
	glm::vec2 position, velocity;
	glm::vec4 color;
	GLfloat life;

	Particle() : position(0.0f), velocity(0.0f), color(1.0f), life(0.0f) {}
};

/*
	ParticleGenerator acts as a container for rendering a large number of
	particles by repeatedly spawning and updating particles and killing them
	after a given amount of time.
*/
class ParticleGenerator {
public:
	// Constructor
	ParticleGenerator(Shader shader, Texture2D texture, GLuint amount);
	// Update all particles
	void update(GLfloat dt, GameObject &object, GLuint newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
	// Render all particles
	void draw();
private:
	// State
	std::vector<Particle> particles;
	GLuint amount;
	// Render state
	Shader shader;
	Texture2D texture;
	GLuint VAO;
	// Initialises buffer and vertex attributes
	void init();
	// Returns the first particle index that's currently unused
	GLuint firstUnusedParticle();
	// Respawns particle
	void respawnParticle(Particle &particle, GameObject &object, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};

#endif // !PARTICLE_GENERATOR_H


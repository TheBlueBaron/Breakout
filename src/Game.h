#ifndef GAME_H
#define GAME_H

#include <vector>
#include <tuple>

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "GameObject.h"
#include "GameLevel.h"
#include "PowerUp.h"


//	Represents the current game state
enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

typedef std::tuple<GLboolean, Direction, glm::vec2> Collision;

// Initialise the size of the player paddle
const glm::vec2 PLAYER_SIZE(100, 20);
// Initialise the velocity of the player paddle
const GLfloat PLAYER_VELOCITY(500.0f);
// Initial velocity of the ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// Radius of the ball object
const GLfloat BALL_RADIUS = 12.5f;

/*	
	Game class contains all game related states and functionality
	All game related data compiled into a single class for
	easy access to components and manageability
*/
class Game {
public:
	// Game state
	GameState state;
	GLboolean keys[1024];
	GLboolean keysProcessed[1024];
	GLuint width, height;
	std::vector<GameLevel> levels;
	std::vector<PowerUp> powerUps;
	GLuint level;
	GLuint lives;
	// Constuctor / Destructor
	Game(GLuint width, GLuint height);
	~Game();
	// Initialise game state
	void init();
	// Game loop
	void processInput(GLfloat dt);
	void update(GLfloat dt);
	void render();
	void doCollisions();
	void spawnPowerUps(GameObject &block);
	void updatePowerUps(GLfloat dt);
private:
	GLboolean checkCollision(GameObject &one, GameObject &two);
	Collision checkCollision(BallObject &one, GameObject &two);
	Direction vectorDirection(glm::vec2 target);
	void resetLevel();
	void resetPlayer();
};

#endif // !GAME_H


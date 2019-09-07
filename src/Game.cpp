#include <algorithm>
#include <sstream>

#include "irrKlang/include/irrKlang.h"
using namespace irrklang;

#include "Game.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"
#include "GameObject.h"
#include "BallObject.h"
#include "ParticleGenerator.h"
#include "PostProcessor.h"
#include "TextRenderer.h"

// Game related state data
SpriteRenderer *renderer;
GameObject *player;
BallObject *ball;
ParticleGenerator *particles;
PostProcessor *effects;
ISoundEngine *soundEngine = createIrrKlangDevice();
TextRenderer *text;
GLfloat shakeTime = 0.0f;

GLboolean isOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type);
void activatePowerUp(PowerUp &powerUp);

Game::Game(GLuint width, GLuint height)
	:state(GAME_MENU), keys(), width(width), height(height), lives(3) {
}

Game::~Game() {
	delete renderer;
	delete player;
	delete ball;
	delete particles;
}

void Game::init() {
	// Load shaders
	ResourceManager::loadShader("res/shaders/sprite.vs", "res/shaders/sprite.frag", nullptr, "sprite");
	ResourceManager::loadShader("res/shaders/particle.vs", "res/shaders/particle.frag", nullptr, "particle");
	ResourceManager::loadShader("res/shaders/postProcess.vs", "res/shaders/postProcess.frag", nullptr, "postprocessing");
	// Conficgure shaders
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->width), static_cast<GLfloat>(this->height), 0.0f, -1.0f, 1.0f);
	ResourceManager::getShader("sprite").use().setInteger("image", 0);
	ResourceManager::getShader("sprite").setMatrix4("projection", projection);
	ResourceManager::getShader("particle").use().setInteger("sprite", 0);
	ResourceManager::getShader("particle").setMatrix4("projection", projection);
	// Load textures
	ResourceManager::loadTexture("res/img/background.jpg", GL_FALSE, "background");
	ResourceManager::loadTexture("res/img/ball.png", GL_TRUE, "ball");
	ResourceManager::loadTexture("res/img/block.png", GL_FALSE, "block");
	ResourceManager::loadTexture("res/img/block_solid.png", GL_FALSE, "block_solid");
	ResourceManager::loadTexture("res/img/paddle.png", GL_TRUE, "paddle");
	ResourceManager::loadTexture("res/img/particle.png", GL_TRUE, "particle");
	ResourceManager::loadTexture("res/img/powerup_speed.png", GL_TRUE, "texSpeed");
	ResourceManager::loadTexture("res/img/powerup_sticky.png", GL_TRUE, "texSticky");
	ResourceManager::loadTexture("res/img/powerup_increase.png", GL_TRUE, "texSizeIncrease");
	ResourceManager::loadTexture("res/img/powerup_confuse.png", GL_TRUE, "texConfuse");
	ResourceManager::loadTexture("res/img/powerup_chaos.png", GL_TRUE, "texChaos");
	ResourceManager::loadTexture("res/img/powerup_passthrough.png", GL_TRUE, "texPassThrough");
	// Set render specific controls
	renderer = new SpriteRenderer(ResourceManager::getShader("sprite"));
	particles = new ParticleGenerator(ResourceManager::getShader("particle"), ResourceManager::getTexture("particle"), 500);
	effects = new PostProcessor(ResourceManager::getShader("postprocessing"), this->width, this->height);
	// Load font
	text = new TextRenderer(this->width, this->height);
	// Font name can be lowercase but extension has to be .ttf (lowercase).
	// Tested with all uppercase and combinations of upper and lower extensions and there doesn't seem to be a problem.
	text->load("res/fonts/ocraext.ttf", 24);
	// Load levels
	GameLevel one; one.load("res/levels/one.lvl", this->width, this->height * 0.5);
	GameLevel two; two.load("res/levels/two.lvl", this->width, this->height * 0.5);
	GameLevel three; three.load("res/levels/three.lvl", this->width, this->height * 0.5);
	GameLevel four; four.load("res/levels/four.lvl", this->width, this->height * 0.5);
	this->levels.push_back(one);
	this->levels.push_back(two);
	this->levels.push_back(three);
	this->levels.push_back(four);
	this->level = 0;
	// Configure game objects
	glm::vec2 playerPos = glm::vec2(this->width / 2 - PLAYER_SIZE.x / 2, this->height - PLAYER_SIZE.y);
	player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::getTexture("paddle"));
	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
	ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::getTexture("ball"));
	// Audio
	soundEngine->play2D("res/sound/breakout.mp3", GL_TRUE);
}

void Game::processInput(GLfloat dt) {
	if (this->state == GAME_MENU) {
		if (this->keys[GLFW_KEY_ENTER] && !this->keysProcessed[GLFW_KEY_ENTER]) {
			this->state = GAME_ACTIVE;
			this->keysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
		}
		if (this->keys[GLFW_KEY_W] && !this->keysProcessed[GLFW_KEY_W]) {
			if (this->level < 3) {
				++this->level;
			}
			else {
				this->level = 0;				
			}
			this->keysProcessed[GLFW_KEY_W] = GL_TRUE;
		}
		if (this->keys[GLFW_KEY_S] && !this->keysProcessed[GLFW_KEY_S]) {
			if (this->level > 0) {
				--this->level;
			}
			else {
				this->level = 3;
			}
			this->keysProcessed[GLFW_KEY_S] = GL_TRUE;
		}
	}
	if (this->state == GAME_WIN) {
		if (this->keys[GLFW_KEY_ENTER]) {
			this->keysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
			effects->chaos = GL_FALSE;
			this->state = GAME_MENU;
		}
	}
	if (this->state == GAME_ACTIVE) {
		GLfloat velocity = PLAYER_VELOCITY * dt;
		// Move playerboard
		if (this->keys[GLFW_KEY_A]) {
			if (player->position.x >= 0) {
				player->position.x -= velocity;
			}
		}
		if (this->keys[GLFW_KEY_D]) {
			if (player->position.x <= this->width - player->size.x) {
				player->position.x += velocity;
			}
		}
		if (this->keys[GLFW_KEY_SPACE]) {
			ball->stuck = false;
		}
	}	
}

void Game::update(GLfloat dt) {
	if (ball->stuck) {
		ball->position = glm::vec2(player->position.x + player->size.x / 2 - ball->radius, -PLAYER_SIZE.y + this->height + -ball->radius * 2);
	}
	// Update objects
	ball->move(dt, this->width);
	// Check collisions
	this->doCollisions();
	// Update particles
	particles->update(dt, *ball, 2, glm::vec2(ball->radius / 2));
	// Update powerUps
	this->updatePowerUps(dt);
	// Does the ball hit the bottom edge
	if (ball->position.y >= this->height) {
		--this->lives;
		// Player lives == 0?
		if (this->lives == 0) {
			this->resetLevel();
			this->state = GAME_MENU;
		}
		this->resetPlayer();
	}
	// Check win condition
	if (this->state == GAME_ACTIVE && this->levels[this->level].isCompleted()) {
		this->resetLevel();
		this->resetPlayer();
		effects->chaos = GL_TRUE;
		this->state = GAME_WIN;
	}
	// Reduce shake time
	if (shakeTime > 0.0f) {
		shakeTime -= dt;
		if (shakeTime <= 0.0f) {
			effects->shake = false;
		}
	}
}

void Game::render() {
	if (this->state == GAME_ACTIVE || this->state == GAME_MENU || this->state == GAME_WIN) {
		effects->beginRender();
			// Draw background
			renderer->drawSprite(ResourceManager::getTexture("background"), glm::vec2(0, 0), glm::vec2(this->width, this->height), 0.0f);
			// Draw level
			this->levels[this->level].draw(*renderer);
			// Draw player
			player->draw(*renderer);
			// Draw powerups
			for (PowerUp &powerUp : this->powerUps) {
				if (!powerUp.destroyed) {
					powerUp.draw(*renderer);
				}
			}
			// Draw particles
			particles->draw();
			// Draw ball
			ball->draw(*renderer);
		// End post-processing rendering
		effects->endRender();
		// Render post-processing quad
		effects->render(glfwGetTime());
		// Render text (no-postprocessing)
		std::stringstream ss; ss << this->lives;
		text->renderText("Lives:" + ss.str(), 5.0f, 5.0f, 1.0f);
	}
	if (this->state == GAME_MENU) {
		text->renderText("Press ENTER to start", 250.0f, height / 2, 1.0f);
		text->renderText("Press W or S to select level", 245.0f, height / 2 + 20.0f, 0.75f);
	}
	if (this->state == GAME_WIN) {
		text->renderText("You WON!!!", 320.0, height / 2 - 20.0, 1.0, glm::vec3(0.0, 1.0, 0.0));
		text->renderText("Press ENTER to retry or ESC to quit", 130.0, height / 2, 1.0, glm::vec3(1.0, 1.0, 0.0));
	}
}

GLboolean Game::checkCollision(GameObject &one, GameObject &two) {
	// Collision x-axis check
	bool collisionX = one.position.x + one.size.x >= two.position.x &&
		two.position.x + two.size.x >= one.position.x;
	// Collision  y-axis check
	bool collisionY = one.position.y + one.size.y >= two.position.y &&
		two.position.y + two.size.y >= one.position.y;
	// Collision only of both true
	return collisionX && collisionY;
}

Collision Game::checkCollision(BallObject &one, GameObject &two) {
	// Get centre point of circle
	glm::vec2 centre(one.position + one.radius);
	// Calculate AABB info (centre, half-extents)
	glm::vec2 aabbHalfExtentes(two.size.x / 2, two.size.y / 2);
	glm::vec2 aabbcentre(two.position.x + aabbHalfExtentes.x, two.position.y + aabbHalfExtentes.y);
	// Get the difference between both centres
	glm::vec2 difference = centre - aabbcentre;
	glm::vec2 clamped = glm::clamp(difference, -aabbHalfExtentes, aabbHalfExtentes);
	// Add clamped value to aabbcentre
	glm::vec2 closest = aabbcentre + clamped;
	// Retrieve vector between circle and closest point AABB
	difference = closest - centre;
	
	if (glm::length(difference) <= one.radius) {
		return std::make_tuple(GL_TRUE, vectorDirection(difference), difference);
	}
	else {
		return std::make_tuple(GL_FALSE, UP, glm::vec2(0, 0));
	}
}

Direction Game::vectorDirection(glm::vec2 target) {
	glm::vec2 compass[]{
		glm::vec2( 0.0f,  1.0f),		// up
		glm::vec2( 1.0f,  0.0f),		// right
		glm::vec2( 0.0f, -1.0f),		// down
		glm::vec2(-1.0f,  0.0f)		// left
	};
	GLfloat max = 0.0f;
	GLuint bestMatch = -1;
	for (GLuint i = 0; i < 4; i++) {
		GLfloat dotProduct = glm::dot(glm::normalize(target), compass[i]);
		if (dotProduct > max) {
			max = dotProduct;
			bestMatch = i;
		}
	}
	return (Direction)bestMatch;
}

void Game::resetLevel() {
	switch (this->level) {
		case 0: {
			this->levels[0].load("res/levels/one.lvl", this->width, this->height * 0.5f);
			break;
		}
		case 1: {
			this->levels[1].load("res/levels/two.lvl", this->width, this->height * 0.5f);
			break;
		}
		case 2: {
			this->levels[2].load("res/levels/three.lvl", this->width, this->height * 0.5f);
			break;
		}
		case 3: {
			this->levels[3].load("res/levels/four.lvl", this->width, this->height * 0.5f);
			break;
		}
		default:
			break;
	}
	this->lives = 3;
}

void Game::resetPlayer() {
	// Reset player and ball
	player->size = PLAYER_SIZE;
	player->position = glm::vec2(this->width / 2 - PLAYER_SIZE.x / 2, this->height - PLAYER_SIZE.y);
	ball->reset(player->position + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -(BALL_RADIUS * 2)), INITIAL_BALL_VELOCITY);
	// Reset active powerups
	effects->chaos = effects->confuse = GL_FALSE;
	player->color = glm::vec3(1.0f);
	ball->color = glm::vec3(1.0f);
	this -> powerUps.clear();
}

void Game::doCollisions() {
	for (GameObject &box : this->levels[this->level].bricks) {
		if (!box.destroyed) {
			Collision collision = checkCollision(*ball, box);
			if (std::get<0>(collision)) { // If collision is true
				// Destroy box if not solid
				if (!box.isSolid) {
					box.destroyed = GL_TRUE;
					this->spawnPowerUps(box);
					soundEngine->play2D("res/sound/bleep.wav", GL_FALSE);
				}
				else { // If box is solid, enable shake effect
					shakeTime = 0.05f;
					effects->shake = true;
					soundEngine->play2D("res/sound/solid.wav", GL_FALSE);
				}
				// Collision resolution
				Direction dir = std::get<1>(collision);
				glm::vec2 diffVector = std::get<2>(collision);
				if (!(ball->passThrough && !box.isSolid)) { // No collision resolution on non-solid bricks when pass-through is active
					if (dir == LEFT || dir == RIGHT) { // Horizontal collision
						ball->velocity.x = -ball->velocity.x; // Reverse horizontal velocity
						// Relocate
						GLfloat penetration = ball->radius - std::abs(diffVector.x);
						if (dir == LEFT) {
							ball->position.x += penetration; // Move ball right
						}
						else {
							ball->position.x -= penetration; // Move ball left
						}
					}
					else { // Vertical collision
						ball->velocity.y = -ball->velocity.y; // Reverse vertical velocity
						// Relocate
						GLfloat penetration = ball->radius - std::abs(diffVector.y);
						if (dir == UP) {
							ball->position.y -= penetration; // Move ball up
						}
						else {
							ball->position.y += penetration; // Move ball down
						}
					}
				}
			}
		}
	}

	for (PowerUp &powerUp : this->powerUps) {
		if (!powerUp.destroyed) {
			if (powerUp.position.y >= height) {
				powerUp.destroyed = GL_TRUE;
			}
			if (checkCollision(*player, powerUp)) {
				// Collided with player, activate powerUp
				activatePowerUp(powerUp);
				powerUp.destroyed = GL_TRUE;
				powerUp.activated = GL_TRUE;
				soundEngine->play2D("res/sound/powerup.wav", GL_FALSE);
			}
		}
	}

	Collision result = checkCollision(*ball, *player);
	if (!ball->stuck && std::get<0>(result)) {
		// Check where it hit the paddle, and change velocity based on where it hit
		GLfloat centreBoard = player->position.x + player->size.x / 2;
		GLfloat distance = (ball->position.x + ball->radius) - centreBoard;
		GLfloat percentage = distance / (player->size.x / 2);
		// Move accordingly
		GLfloat strength = 2.0f;
		glm::vec2 oldVelocity = ball->velocity;
		ball->velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
		//ball->velocity.y = -ball->velocity.y;
		ball->velocity.y = -1 * abs(ball->velocity.y);
		ball->velocity = glm::normalize(ball->velocity) * glm::length(oldVelocity);

		// If sticky powerUp is activated, also stick the ball to paddle once new velocity vectors have been calculated
		ball->stuck = ball->sticky;
		soundEngine->play2D("res/sound/bleep_paddle.wav", GL_FALSE);
	}
}

GLboolean shouldSpawn(GLuint chance) {
	GLuint random = rand() % chance;
	return random == 0;
}

void Game::spawnPowerUps(GameObject & block) {
	if (shouldSpawn(75)) { // 1 in 75 chance
		this->powerUps.push_back(PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.position, ResourceManager::getTexture("texSpeed")));
	}
	if (shouldSpawn(75)) {
		this->powerUps.push_back(PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 7.0f, block.position, ResourceManager::getTexture("texSticky")));
	}
	if (shouldSpawn(75)) {
		this->powerUps.push_back(PowerUp("passThrough", glm::vec3(0.5f, 1.0f, 0.5f), 5.0f, block.position, ResourceManager::getTexture("texPassThrough")));
	}
	if (shouldSpawn(75)) {
		this->powerUps.push_back(PowerUp("sizeIncrease", glm::vec3(1.0f, 0.6f, 0.4f), 0.0f, block.position, ResourceManager::getTexture("texSizeIncrease")));
	}
	if (shouldSpawn(15)) {
		this->powerUps.push_back(PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 5.0f, block.position, ResourceManager::getTexture("texConfuse")));
	}
	if (shouldSpawn(15)) {
		this->powerUps.push_back(PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 5.0f, block.position, ResourceManager::getTexture("texChaos")));
	}
}

void activatePowerUp(PowerUp &powerUp) {
	// Initiate a powerUp based on type of powerUp
	if (powerUp.type == "speed") {
		ball->velocity *= 1.2;
	}
	else if (powerUp.type == "sticky") {
		ball->sticky = GL_TRUE;
		player->color = glm::vec3(1.0f, 0.5f, 1.0f);
	}
	else if (powerUp.type == "passThrough") {
		ball->passThrough = GL_TRUE;
		ball->color = glm::vec3(1.0f, 0.5f, 0.5f);
	}
	else if (powerUp.type == "sizeIncrease") {
		player->size.x += 50;
	}
	else if (powerUp.type == "confuse") {
		if (!effects->chaos) {
			effects->confuse = GL_TRUE; // Only activate if chaos wasn't already active
		}
	}
	else if (powerUp.type == "chaos") {
		if (!effects->confuse) {
			effects->chaos = GL_TRUE;
		}
	}
}



void Game::updatePowerUps(GLfloat dt) {
	for (PowerUp &powerUp : this->powerUps) {
		powerUp.position += powerUp.velocity * dt;
		if (powerUp.activated) {
			powerUp.duration -= dt;

			if (powerUp.duration <= 0.0f) {
				powerUp.activated = GL_FALSE;
				// Deactivate effects
				if (powerUp.type == "sticky") {
					if (!isOtherPowerUpActive(this->powerUps, "sticky")) {
						// Only reset if no other powerUp of type sticky is active
						ball->sticky = GL_FALSE;
						player->color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.type == "passThrough") {
					if(!isOtherPowerUpActive(this->powerUps, "passThrough")){
						ball->passThrough = GL_FALSE;
						ball->color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.type == "confuse") {
					if (!isOtherPowerUpActive(this->powerUps, "confuse")) {
						effects->confuse = GL_FALSE;
					}
				}
				else if (powerUp.type == "chaos") {
					if (!isOtherPowerUpActive(this->powerUps, "chaos")) {
						effects->chaos = GL_FALSE;
					}
				}
			}
		}
	}

	// Remove all PowerUps from vector that are destroyed and not active.  Lambda expression used to remove each PowerUp that meets those conditions
	this->powerUps.erase(std::remove_if(this->powerUps.begin(), this->powerUps.end(), [](const PowerUp &powerUp) { return powerUp.destroyed && !powerUp.activated; }), this->powerUps.end());
}

GLboolean isOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type) {
	for (const PowerUp &powerUp : powerUps) {
		if (powerUp.activated) {
			if (powerUp.type == type) {
				return GL_TRUE;
			}
		}
	}
	return GL_FALSE;
}

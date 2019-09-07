#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "Game.h"
#include "ResourceManager.h"

// GLFW function declarations
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Game screen width
const GLuint SCREEN_WIDTH = 800;
// Game screen height
const GLuint SCREEN_HEIGHT = 600;

Game breakout(SCREEN_WIDTH, SCREEN_HEIGHT);

int main(int argc, char *argv[]) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();
	glGetError(); // Called once to catch glewInit() bug

	glfwSetKeyCallback(window, keyCallback);

	// OpenGL configuration
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Initialise game
	breakout.init();

	// deltaTime variables
	GLfloat deltaTime = 0.0f;
	GLfloat lastFrame = 0.0f;

	// Start game within menu state
	breakout.state = GAME_MENU;

	while (!glfwWindowShouldClose(window)) {
		// Calculate delta time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents();

		//deltaTIme = 0.001f;
		// Manage user input
		breakout.processInput(deltaTime);

		// Update game state
		breakout.update(deltaTime);

		// Render
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		breakout.render();

		glfwSwapBuffers(window);
	}

	// Delete all resources that have been loaded during runtime
	ResourceManager::clear();

	glfwTerminate();
	return 0;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	// When the user presses the excape key, close the appliction
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key <= 1024) {
		if (action == GLFW_PRESS) {
			breakout.keys[key] = GL_TRUE;
		}
		else if (action == GLFW_RELEASE) {
			breakout.keys[key] = GL_FALSE;
			breakout.keysProcessed[key] = GL_FALSE;
		}
	}
}
#include "GameLevel.h"

#include <fstream>
#include <sstream>

void GameLevel::load(const GLchar * file, GLuint levelWidth, GLuint levelHeight) {
	// Clear old data
	this->bricks.clear();
	// Load from file
	GLuint tileCode;
	GameLevel level;
	std::string line;
	std::ifstream fstream(file);
	std::vector<std::vector<GLuint>> tileData;

	if (fstream) {
		while (std::getline(fstream, line)) { // Read each line from level file
			std::istringstream sstream(line);
			std::vector<GLuint> row;
			while (sstream >> tileCode) { // Read each word seperated by spaces
				row.push_back(tileCode);
			}
			tileData.push_back(row);
		}
		if (tileData.size() > 0) {
			this->init(tileData, levelWidth, levelHeight);
		}
	}
}

void GameLevel::draw(SpriteRenderer & renderer) {
	for (GameObject &tile : this->bricks) {
		if (!tile.destroyed) {
			tile.draw(renderer);
		}
	}
}

GLboolean GameLevel::isCompleted() {
	for (GameObject &tile : this->bricks) {
		if (!tile.isSolid && !tile.destroyed) {
			return GL_FALSE;
		}
	}

	return GL_TRUE;
}

void GameLevel::init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight) {
	// Calculate dimensions
	GLuint height = tileData.size();
	GLuint width = tileData[0].size();
	GLfloat unitWidth = levelWidth = levelWidth / static_cast<GLfloat>(width), unitHeight = levelHeight / height;
	// Initialise level tiles based in tileData
	for (GLuint y = 0; y < height; y++) {
		for (GLuint x = 0; x < width; x++) {
			// Check block type from level data
			if (tileData[y][x] == 1) { // Solid
				glm::vec2 pos(unitWidth *  x, unitHeight * y);
				glm::vec2 size(unitWidth, unitHeight);
				GameObject obj(pos, size, ResourceManager::getTexture("block_solid"), glm::vec3(0.8f, 0.8f, 0.7f));
				obj.isSolid = GL_TRUE;
				this->bricks.push_back(obj);
			}
			else if (tileData[y][x] > 1) { // Non-solid, color determined by level data
				glm::vec3 color;
				switch (tileData[y][x]) {
					case 2: {
						color = glm::vec3(0.2f, 0.6f, 1.0f);
						break;
						}
					case 3: {
						color = glm::vec3(0.0f, 0.7f, 0.0f);
						break;
					}
					case 4: {
						color = glm::vec3(0.8f, 0.8f, 0.4f);
						break;
					}
					case 5: {
						color = glm::vec3(1.0f, 0.5f, 0.0f);
						break;
					}
					default: {
						color = glm::vec3(1.0f);
					}
				}

				glm::vec2 pos(unitWidth * x, unitHeight * y);
				glm::vec2 size(unitWidth, unitHeight);
				this->bricks.push_back(GameObject(pos, size, ResourceManager::getTexture("block"), color));
			}
		}
	}
}

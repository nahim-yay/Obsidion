
#pragma once




#include "glm/glm.hpp"

struct Spritesheet
{
	const int textureUnit;

	// Currently assumes all images in a single row, uniformly spaced and timed.
	const int numCols;
	const float timePerSprite;

	Spritesheet(int a, int b, float c) : textureUnit(a), numCols(b), timePerSprite(c) {}
};

struct SpritesheetSprite
{
	glm::vec3 worldPos;
	float age;

	explicit SpritesheetSprite(glm::vec3 _worldPos) : worldPos(_worldPos), age(0.0f) {}
};
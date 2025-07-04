#pragma once
#include <random>
#include <glm/vec2.hpp>

// Returns a float in [min, max)
float getRandomFloat(std::ranlux24_base &rng, float min, float max);

// Returns an int in [min, max]
int getRandomInt(std::ranlux24_base &rng, int min, int max);

bool getRandomChance(std::ranlux24_base &rng, float chance);

glm::vec2 getRandomUnitVector(std::ranlux24_base &rng);

uint32_t hashPosition(int x, int y);

//takes a position hashes it and applies it as the starting seed for a rng
std::ranlux24_base rngFromPositions(int x, int y);
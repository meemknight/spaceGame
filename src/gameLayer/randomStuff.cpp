#include "randomStuff.h"
#include <glm/glm.hpp>

// Returns a float in [min, max)
float getRandomFloat(std::ranlux24_base &rng, float min, float max)
{
	std::uniform_real_distribution<float> dist(min, max);
	return dist(rng);
}

// Returns an int in [min, max]
int getRandomInt(std::ranlux24_base &rng, int min, int max)
{
	std::uniform_int_distribution<int> dist(min, max);
	return dist(rng);
}

bool getRandomChance(std::ranlux24_base &rng, float chance)
{
	float dice = getRandomFloat(rng, 0.0, 1.0);
	return dice <= chance;
}

glm::vec2 getRandomUnitVector(std::ranlux24_base &rng)
{
	// Generate a random angle in radians
	std::uniform_real_distribution<float> dist(0.0f, 2.0f * 3.14159265359);
	float angle = dist(rng);

	// Use trigonometric functions to calculate the components of the unit vector
	float x = std::sin(angle);
	float y = std::cos(angle);

	// Return the resulting unit vector
	return glm::normalize(glm::vec2(x, y));
}

uint32_t hashPosition(int x, int y)
{
	uint64_t h = static_cast<uint64_t>(x) * 0x8da6b343 + static_cast<uint64_t>(y) * 0xd8163841;
	h ^= (h >> 33);
	h *= 0xff51afd7ed558ccdULL;
	h ^= (h >> 33);
	h *= 0xc4ceb9fe1a85ec53ULL;
	h ^= (h >> 33);
	return static_cast<uint32_t>(h);
}

std::ranlux24_base rngFromPositions(int x, int y)
{
	return std::ranlux24_base(hashPosition(x, y));
}

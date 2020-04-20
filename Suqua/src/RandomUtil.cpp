#include "RandomUtil.h"

int randInt(int min, int max) {
	static std::random_device rd;
	static std::default_random_engine eng;
	static bool toSeed{ true };
	if (toSeed) {
		eng.seed(rd());
		toSeed = false;
	}

	std::uniform_int_distribution<int> dist{ min, max };
	int number = dist(eng);
	return number;
}

float randFloat(float min, float max) {
	static std::random_device rd;
	static std::default_random_engine eng;
	static bool toSeed{ true };
	if (toSeed) {
		eng.seed(rd());
		toSeed = false;
	}

	std::uniform_real_distribution<float> dist{ min, max };
	float number = dist(eng);
	return number;
}
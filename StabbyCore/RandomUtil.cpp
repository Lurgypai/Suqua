#include "stdafx.h"
#include "RandomUtil.h"

 int random(int min, int max) {
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
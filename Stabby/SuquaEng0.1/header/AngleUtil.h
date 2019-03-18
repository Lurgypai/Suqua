#pragma once
#include "stdafx.h"
#define PI 3.1415926535898

inline float formatAngle(float angle) {
	while (angle < 0)
		angle += 2 * PI;
	while (angle >= 2 * PI)
		angle -= 2 * PI;

	return angle;
}

inline float degrees(float angle) {
	angle *= 180 / PI;

	return angle;
}

inline float radians(float angle) {
	angle *= PI / 180;

	return angle;
}
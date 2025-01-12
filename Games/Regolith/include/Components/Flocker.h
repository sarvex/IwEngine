#pragma once

#include "glm/vec2.hpp"

struct Flocker {
	bool Active = true;
	glm::vec2 Target = glm::vec2(0.f);

	float Speed = 100;
	float SpeedNearTarget = 1;

	float ForceWeight = 1; // % of calculated force applied

	float AwayRadius = 1000;

	//int FlockingWith = 0; // always 0, but then only another id
}; // should have strength of flocking forces

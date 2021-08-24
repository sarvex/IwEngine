#pragma once

#include "plugins/iw/Sand/SandChunk.h"
#include <utility>
#include <functional>

enum ProjectileType {
	BULLET, // these are the same actually
	LASER,
	BEAM
};

struct Projectile {
	std::function<void()> FixedUpdate; // called in physics tick
	std::function<std::tuple<bool, float, float>()> Update; // called in render tick, return true if collision

	std::function<void(float, float)> OnHit; // called on collision

	std::function<void(iw::SandChunk*, int, int, float, float)> PlaceCell; // each cell in line calls this
};

struct ShotInfo {
	float x, y, dx, dy;
	ProjectileType projectile;

	float Speed() {
		return sqrt(dx*dx + dy*dy);
	}
};

inline ShotInfo GetShot_Circular(
	float x,  float y,
	float tx, float ty,
	float speed, float margin, float thickness = 0)
{
	float dx = tx - x,
	      dy = ty - y;

	float length = sqrt(dx*dx + dy*dy);

	float nx = dx / length,
           ny = dy / length;

	dx = nx * speed;
	dy = ny * speed;

	float r = iw::randfs();

	x += nx * margin - ny * thickness * r;
	y += ny * margin + nx * thickness * r;

	return ShotInfo { x, y, dx, dy };
}

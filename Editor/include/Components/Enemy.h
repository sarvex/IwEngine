#pragma once

#include "Bullet.h"

enum class REFLECT EnemyType {
	SPIN               = 0x0,
	CIRCLE,
	SEEK,
	MINI_BOSS_BOX_SPIN = 0x10,
	BOSS_FOREST        = 0x100
};

struct REFLECT Enemy {
	REFLECT EnemyType Type;
	REFLECT Bullet Bullet;
	REFLECT float Speed;
	REFLECT float FireTime;
	REFLECT float ChargeTime;
	REFLECT float Rotation;

	float Timer;
	bool  HasShot;

	// only for boss
	float Timer2;

	// only for circle might get removed
	float RotSpeed;

	// only really for bosses
	int Health;
	int ScoreMultiple;

	bool JustHit;
};

#pragma once

#include "iw/entity/Entity.h"
#include "iw/engine/Components/Timer.h"
#include "Weapons.h"
#include "ECS.h"

enum EnemyType
{
	FIGHTER,
	BOMB,
	STATION,
	BASE,
	BOSS_1
};

struct Enemy
{
	//iw::Entity Target;
	entity Target;
	float ExplosionPower = 10.f;

	float ActiveTimer = 0.f;
	float ActiveTime  = .1f;
};

struct Fighter_Enemy
{
	Weapon* Weapon = nullptr;
};

struct Bomb_Enemy
{
	float TimeToExplode = 1.f;
	float RadiusToExplode = 10.f;
	bool Explode = false;
};

struct Station_Enemy
{
	iw::Timer Timer;
};

struct Base_Enemy
{

};

struct Boss_1_Enemy
{
	iw::Timer Timer;
	Weapon* EnergyBall = nullptr;
};

#include "Systems/Enemy_System.h"

void EnemySystem::FixedUpdate()
{
	Space->Query<iw::Rigidbody, Enemy>().Each(
		[](
			iw::EntityHandle handle, 
			iw::Rigidbody* body, 
			Enemy* ship) 
		{
			if (body->IsTrigger)
			{
				ship->ActiveTimer += iw::FixedTime();

				if (ship->ActiveTimer > ship->ActiveTime)
				{
					body->IsTrigger = false;
				}
			}
		});

	Space->Query<iw::Transform, Enemy, Fighter_Enemy>().Each(
		[&](
			iw::EntityHandle handle,
			iw::Transform* transform,
			Enemy*         ship,
			Fighter_Enemy* fighter)
		{
			if (!ship->Target || !ship->Target.Alive())
			{
				return;
			}
			
			if (   abs(transform->Position.x - 200) > 200
				|| abs(transform->Position.y - 200) > 200)
			{
				return;
			}

			if (!fighter->Weapon->CanFire())
			{
				return;
			}

			glm::vec3 pos = ship->Target.Find<iw::Transform>()->Position;
			ShotInfo shot = fighter->Weapon->GetShot(Space->GetEntity(handle), pos.x, pos.y);
			Bus->push<SpawnProjectile_Event>(shot);

			Audio->Play("event:/weapons/fire_laser");
		}
	);

	Space->Query<iw::Transform, iw::Tile, Flocker, Enemy, Bomb_Enemy>().Each(
		[&](
			iw::EntityHandle handle,
			iw::Transform* transform,
			iw::Tile* tile,
			Flocker* flocker,
			Enemy* ship,
			Bomb_Enemy* bomb)
		{
			// maybe put some timer or something

			if (!ship->Target || !ship->Target.Alive())
			{
				return;
			}

			glm::vec3 pos = ship->Target.Find<iw::Transform>()->Position;
			
			// set flocking target to target
			flocker->Target = glm::vec2(pos.x, pos.y);

			// start explosion if close to target
			if (   !bomb->Explode 
				&& glm::distance(pos, transform->Position) < bomb->RadiusToExplode)
			{
				bomb->Explode = true;
			}

			if (bomb->Explode)
			{
				bomb->TimeToExplode -= iw::FixedTime();
				
				tile->m_tint = iw::Color(1, .1, .1);

				if (bomb->TimeToExplode <= 0.f)
				{
					SpawnExplosion_Config config;
					config.SpawnLocationX = transform->Position.x;
					config.SpawnLocationY = transform->Position.y;
					config.ExplosionPower = 900;
					config.ExplosionRadius = 10;

					Bus->push<SpawnExplosion_Event>(config);
					Space->QueueEntity(handle, iw::func_Destroy);
				}
			}
		}
	);

	Space->Query<iw::Transform, Enemy, Station_Enemy>().Each(
		[&](
			iw::EntityHandle handle,
			iw::Transform* transform,
			Enemy*         ship,
			Station_Enemy* station)
		{
			if (!ship->Target || !ship->Target.Alive())
			{
				return;
			}

			station->timer.TickFixed();
			if (station->timer.Can("spawn"))
			{
				SpawnEnemy_Config config;

				glm::vec3 right = transform->Right();
				switch (iw::randi(2))
				{
					case 0: right = -right;                             break;
					case 1: right = glm::vec3(-right.y,  right.x, 0.f); break;
					case 2: right = glm::vec3( right.y, -right.y, 0.f); break;
					default:                                            break;
				}

				right *= 25.f;

				config.SpawnLocationX = transform->Position.x + right.x;
				config.SpawnLocationY = transform->Position.y + right.y;
				config.EnemyType = iw::randf() > .4 ? BOMB : FIGHTER;
				config.TargetEntity = ship->Target;

				Bus->push<SpawnEnemy_Event>(config);
			}
		}
	);

	Space->Query<iw::Transform, Enemy, Base_Enemy>().Each(
		[&](
			iw::EntityHandle handle,
			iw::Transform* transform,
			Enemy*         ship,
			Base_Enemy*    base)
		{
			if (!ship->Target.Alive())
			{
				return;
			}

			Throwable* closestThrowable = nullptr;
			float closestDist = FLT_MAX;
			Space->Query<iw::Transform, Throwable>().Each([&](
				iw::EntityHandle handleThrowable,
				iw::Transform* transformThrowable,
				Throwable* throwable)
			{
				float dist = glm::distance(transform->Position, transformThrowable->Position);

				if (   dist < 150 
					&& dist < closestDist)
				{
					closestDist = dist;
					closestThrowable = throwable;
				}
			});

			if (    closestThrowable 
				&& !closestThrowable->Held)
			{
				closestThrowable->Held = true;
				closestThrowable->Time  = 1.f;
				closestThrowable->Timer = 0.f;
				closestThrowable->ThrowRequestor = Space->GetEntity(handle);
				closestThrowable->ThrowTarget = ship->Target;
			}
		}
	);
}

bool EnemySystem::On(iw::ActionEvent& e)
{
	switch (e.Action)
	{
		case SPAWN_ENEMY:
		{
			SpawnEnemy(e.as<SpawnEnemy_Event>().Config);
			break;
		}
		case CORE_EXPLODED: 
		{
			iw::Entity& entity = e.as<CoreExploded_Event>().Entity;

			if (entity.Has<Enemy>())
			{
				DestroyEnemy(e.as<CoreExploded_Event>().Entity);
			}

			break;
		}
	}

	return false;
}

void EnemySystem::SpawnEnemy(SpawnEnemy_Config& config)
{
	iw::Archetype archetype = Space->CreateArchetype<Flocker, CorePixels, Enemy>();
	iw::Entity entity;

	switch (config.EnemyType)
	{
		case FIGHTER: 
		{
			Space->AddComponent<Fighter_Enemy>(archetype);
			entity = sand->MakeTile<iw::Circle>(*A_texture_enemy_fighter, true, &archetype);
			break;
		}
		case BOMB: 
		{
			Space->AddComponent<Bomb_Enemy>(archetype);
			Space->AddComponent<Throwable>(archetype);

			entity = sand->MakeTile<iw::Circle>(*A_texture_enemy_bomb, true, &archetype);
			break;
		}
		case STATION:
		{
			Space->AddComponent<Station_Enemy>(archetype);
			entity = sand->MakeTile<iw::MeshCollider2>(*A_texture_enemy_station, true, &archetype);
			break;
		}
		case BASE:
		{
			Space->AddComponent<Base_Enemy>(archetype);
			entity = sand->MakeTile<iw::MeshCollider2>(*A_texture_enemy_base, true, &archetype);

			break;
		}
	}
	
	iw::Rigidbody* rigidbody = entity.Find<iw::Rigidbody>();
	iw::Transform* transform = entity.Find<iw::Transform>();
	Flocker*       flocker   = entity.Set<Flocker>();
	Enemy*         enemy     = entity.Set<Enemy>();
	CorePixels*    core      = entity.Set<CorePixels>();

	core->TimeWithoutCore = 0.f;
	
	flocker->Target.x = config.TargetLocationX;
	flocker->Target.y = config.TargetLocationY;

	transform->Position.x = config.SpawnLocationX;
	transform->Position.y = config.SpawnLocationY;

	rigidbody->SetTransform(transform);
	rigidbody->SetMass(10);
	rigidbody->IsTrigger = true;

	enemy->Target = config.TargetEntity;

	switch (config.EnemyType)
	{
		case FIGHTER: 
		{
			Fighter_Enemy* fighter = entity.Set<Fighter_Enemy>();
			fighter->Weapon = MakeLaser_Cannon_Enemy();

			enemy->ExplosionPower = 200;

			break;
		}
		case BOMB: 
		{
			Bomb_Enemy* bomb = entity.Set<Bomb_Enemy>();
			bomb->TimeToExplode = .5f;
			bomb->RadiusToExplode = 30;

			flocker->SpeedNearTarget = 1;
			rigidbody->AngularVelocity.z = .9f;

			enemy->ExplosionPower = 200;

			break;
		}
		case STATION:
		{
			Station_Enemy* station = entity.Set<Station_Enemy>();
			station->timer.SetTime("spawn", 2, .5);

			rigidbody->SetMass(100);
			//rigidbody->AngularVelocity.z = .1f;
			//rigidbody->Transform.Rotation.z = .4;
			flocker->Speed = 25;

			enemy->ExplosionPower = 300;

			break;
		}
		case BASE:
		{
			Base_Enemy* station = entity.Set<Base_Enemy>();

			rigidbody->SetMass(1000);
			rigidbody->AngularVelocity.z = .1f;
			flocker->Speed = 25;

			enemy->ExplosionPower = 1200;

			break;
		}
	}

	Bus->push<CreatedCoreTile_Event>(entity);
}

void EnemySystem::DestroyEnemy(iw::Entity entity)
{
	if (!entity.Has<Enemy>()) return;

	std::vector<std::pair<ItemType, float>> item_weights {
		{ ItemType::HEALTH,         50 },
		{ ItemType::LASER_CHARGE,   50 },
		{ ItemType::WEAPON_MINIGUN, 3 },
		{ ItemType::WEAPON_BOLTZ,   2 },
		{ ItemType::WEAPON_WATTZ,   2 },
	};

	iw::Transform* transform = entity.Find<iw::Transform>();

	SpawnItem_Config config;
	config.Item = iw::choose(item_weights);
	config.X = transform->Position.x;
	config.Y = transform->Position.y;
	config.AngularSpeed = iw::randf() * 25 + 10;

	switch (config.Item)
	{
		case ItemType::HEALTH: 
		case ItemType::LASER_CHARGE:   config.Amount = iw::randi(5) + 1; break;
		
		case ItemType::WEAPON_WATTZ:
		case ItemType::WEAPON_BOLTZ:
		case ItemType::WEAPON_MINIGUN: config.Amount = 1;
	}

	Bus->push<SpawnItem_Event>(config);

	// spawn copy of just display
	iw::Entity tileCopy = sand->MakeTile(
		entity.Find<iw::Tile>()->m_sprite,
		true
	);
	tileCopy.Set<iw::Transform>(*transform);
	tileCopy.Find<iw::Rigidbody>()->SetTransform(transform);



	Space->QueueEntity(entity.Handle, iw::func_Destroy);
}

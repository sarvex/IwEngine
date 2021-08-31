#include "Systems/Enemy_System.h"

void EnemySystem::FixedUpdate()
{
	Space->Query<iw::Transform, EnemyShip>().Each(
		[&](
			iw::EntityHandle handle,
			iw::Transform* transform,
			EnemyShip* enemy)
		{
			if (   enemy->Weapon->CanFire()
				&& enemy->ShootAt.Alive())
			{
				glm::vec3 pos = enemy->ShootAt.Find<iw::Transform>()->Position;
				ShotInfo shot = enemy->Weapon->GetShot(Space->GetEntity(handle), pos.x, pos.y);
				Bus->send<SpawnProjectile_Event>(shot);
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
			SpawnEnemy(e.as<SpawnEnemy_Event>());
			break;
		}
		case RUN_GAME: {
			Space->Query<EnemyShip>().Each([&](iw::EntityHandle handle, EnemyShip*) {
				Space->QueueEntity(handle, iw::func_Destroy);
			});
			break;
		}
	}

	return false;
}

void EnemySystem::SpawnEnemy(SpawnEnemy_Event& config)
{
	iw::Entity e = sand->MakeTile<iw::Circle, EnemyShip, Flocker, CorePixels>("textures/SpaceGame/enemy.png", true);

	CorePixels*    core      = e.Set<CorePixels>();
	iw::Transform* transform = e.Set<iw::Transform>();
	iw::Rigidbody* rigidbody = e.Set<iw::Rigidbody>();
	iw::Circle*    collider  = e.Set<iw::Circle>();

	core->TimeWithoutCore = 0.f;

	transform->Position.x = config.SpawnLocationX;
	transform->Position.y = config.SpawnLocationY;

	rigidbody->SetTransform(transform);

	collider->Radius = 6;

	EnemyShip* s = e.Set<EnemyShip>();
	Flocker*   f = e.Set<Flocker>();

	s->Weapon = MakeLaser_Cannon_Enemy();
	s->ShootAt = config.ShootAt;

	f->Target.x = config.TargetLocationX;
	f->Target.y = config.TargetLocationY;
}

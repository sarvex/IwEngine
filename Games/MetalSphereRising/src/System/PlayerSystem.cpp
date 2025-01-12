#include "Systems/PlayerSystem.h"
#include "Events/ActionEvents.h"
#include "Components/Bullet.h"
#include "iw/engine/Time.h"
#include "iw/input/Devices/Keyboard.h"
#include "iw/audio/AudioSpaceStudio.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "iw/reflection/serialization/JsonSerializer.h"
#include "iw/util/io/File.h"

#include "Components/Enemy.h"

PlayerSystem::PlayerSystem(
	iw::Transform*& worldTransform)
	: System("Player")
	, playerPrefab()
	, m_playerModel(nullptr)
	, m_worldTransform(worldTransform)
{
#ifdef IW_DEBUG
	const char* file = "assets/prefabs/player.json";
	bool fileExists  = iw::FileExists(file);
#else
	const char* file = "assets/prefabs/player.bin";
	bool fileExists  = iw::FileExists(file);
#endif

	if (fileExists) {
#ifdef IW_DEBUG
		iw::JsonSerializer(file).Read(playerPrefab);
#else
		iw::Serializer(file).Read(playerPrefab);
#endif
	}

	else {
		// Default values
		playerPrefab.Speed      = 4.25f;
		playerPrefab.DashTime   = 8 / 60.0f;
		playerPrefab.ChargeTime = 0.2f;
		playerPrefab.Health     = 3;

#ifdef IW_DEBUG
		iw::JsonSerializer(file).Write(playerPrefab);
#else
		iw::Serializer(file).Write(playerPrefab);
#endif
	}
}

int PlayerSystem::Initialize() {
	m_playerModel = Asset->Load<iw::Model>("Player");

	m_player = Space->CreateEntity<iw::Transform, iw::Model, iw::SphereCollider, iw::Rigidbody, Player>();
	
	                        m_player.Set<Player>(playerPrefab);
	                        m_player.Set<iw::Model>(*m_playerModel);
	iw::Transform*      t = m_player.Set<iw::Transform>(iw::vector3(0, 1, 0), iw::vector3(1));
	iw::SphereCollider* s = m_player.Set<iw::SphereCollider>(iw::vector3::zero, 0.75f);
	iw::Rigidbody*      r = m_player.Set<iw::Rigidbody>();

	t->SetParent(m_worldTransform);

	//c->SetMass(1);
	r->SetCol(s);
	r->SetTrans(t);
	r->SetIsStatic(false);
	r->SetSimGravity(false);

	r->SetIsLocked(iw::vector3::unit_y);
	r->SetLock    (iw::vector3::unit_y);

	r->SetOnCollision([&](iw::Manifold& man, float dt) {
		iw::Entity playerEntity, otherEntity;
		if (GetEntitiesFromManifold<Player>(man, playerEntity, otherEntity)) {
			return;
		}

		Player* player = playerEntity.Find<Player>();

		if (otherEntity.Has<Enemy>()) {
			Enemy* enemy = otherEntity.Find<Enemy>();

			if (player->Timer <= 0) {
				return;
			}

			if (enemy->Type >= EnemyType::MINI_BOSS_FOREST) {
				if (player->Transition) return; 

				Player*        playerComp  = playerEntity.Find<Player>();
				iw::Transform* playerTrans = playerEntity.Find<iw::Transform>();
				iw::Rigidbody* playerBody  = playerEntity.Find<iw::Rigidbody>();

				//playerComp->Timer = 0.0f; // causes the enemy to not detect damage
				playerBody->SetIsTrigger(true);

				float distance = 0;
				switch (enemy->Type) {
					case EnemyType::MINI_BOSS_CANYON:
					case EnemyType::MINI_BOSS_FOREST: distance = 12; break;
					case EnemyType::BOSS_FOREST:      distance = 18; break;
					default: return;
				}

				Space->QueueChange(&player->Transition, true);

				player->TransitionSpeed = 1.5f;
				player->TransitionStartPosition  = playerTrans->Position;
				player->TransitionTargetPosition = playerTrans->Position + player->Movement.normalized() * distance;
				player->Begin = iw::Time::TotalTime();
			}
		}
		
		else if (otherEntity.Has<Bullet>()) {
			if (   true //!p->Damaged // stop taking more than 1 damage per frame
				&& player->Health > 0
				/*&& !otherEntity.Find<Bullet>()->Die*/)
			{
				player->Damaged = true;
				player->Health -= 1;

				float color = player->Health / 3.0f;
				m_playerModel->GetMesh(0).Material()->Set("baseColor", iw::Color(0.8f, color, color, 1));

				Audio->AsStudio()->CreateInstance("User/playerDamaged");
			}
		}

		else {
			iw::Rigidbody* playerBody = playerEntity.Find<iw::Rigidbody>();

			player->Transition = false; // if hit a wall
			playerBody->SetIsTrigger(false);
		}
	});

	//c->SetStaticFriction (0.0f);
	//c->SetDynamicFriction(0.0f);

	//c->SetIsLocked(iw::vector3(0, 1, 0));
	//c->SetLock    (iw::vector3(0, 1, 0));

	Physics->AddRigidbody(r);

	return 0;
}

float distance; //tmp debug
iw::vector3 start;

void PlayerSystem::Update(
	iw::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [transform, body, player] = entity.Components.Tie<Components>();

		if (player->Transition) {
			iw::vector3& position = body->Trans().Position;

			float time = player->TransitionSpeed * (iw::Time::TotalTime() - player->Begin);

			position = iw::lerp(
				player->TransitionStartPosition,
				player->TransitionTargetPosition,
				1 - (1 - time) * (1 - time)
			);

			if (time > 1) {
				player->Transition = false;
			}
		}

		else {
			if (player->DeathTimer > 0) {
				body->Trans().Scale -= .75f / 1.0f * iw::Time::DeltaTime();
				body->SetVelocity(0.0f);

				player->DeathTimer -= iw::Time::DeltaTime();
				if (player->DeathTimer <= 0) {
					player->DeathTimer = 0;
					Bus->push<ResetLevelEvent>();
				}
			}

			else {
				player->Movement = 0;
				if (player->Left)  player->Movement.x -= 1;
				if (player->Right) player->Movement.x += 1;
				if (player->Up)    player->Movement.z -= 1;
				if (player->Down)  player->Movement.z += 1;

				player->Movement.normalize();
				player->Movement *= player->Speed;

				if (player->Timer > 0) {
					player->Movement *= 10 * player->Timer / player->DashTime;
				}

				if (player->Sprint) {
					player->Movement *= 2.0f;
				}

				//body->Trans().Position += movement * iw::Time::DeltaTime();

				iw::vector3 velocity = body->Velocity();
				velocity.x = player->Movement.x / iw::TimeScale();
				velocity.z = player->Movement.z / iw::TimeScale();

				body->SetVelocity(velocity);

				if (player->Timer <= -player->ChargeTime) {
					if (distance == 0) {
						distance = (start - transform->Position).length();
						LOG_INFO << distance;
					}

					if (   player->Dash
						&& player->Speed > 0.0f
						&& (player->Up || player->Down || player->Left || player->Right))
					{
						start = transform->Position;
						distance = 0;

						player->Timer = player->DashTime;
						Audio->AsStudio()->CreateInstance("User/playerAttack");
					}
				}

				// sprinting after holding x?
				//if (player->Timer <= 0.0f) {
				//	if (dash
				//		|| !sprint)
				//	{
				//		sprint = dash;
				//	}
				//}

				else if (player->Timer >= -player->ChargeTime) {
					player->Timer -= iw::Time::DeltaTime();
				}

				if (player->Health <= 0) {
					if (player->DeathTimer == 0) {
						player->DeathTimer = 1.0f;

						//Audio->PlaySound("Death.wav");
					}
				}

				player->Damaged = false;
			}
		}
	}
}

void PlayerSystem::OnPush() {
	if (!m_player) {
		return;
	}

	Player* player = m_player.Find<Player>();

	player->Left       = false;
	player->Right      = false;
	player->Up         = false;
	player->Down       = false;
	player->Dash       = false;
	player->Sprint     = false;
	player->Transition = false;
}

bool PlayerSystem::On(
	iw::KeyEvent& event)
{
	if (!m_player) {
		return false;
	}

	Player* player = m_player.Find<Player>();

	switch (event.Button) {
		case iw::UP: {
			player->Up = event.State;
			break;
		}
		case iw::DOWN: {
			player->Down = event.State;
			break; 
		}
		case iw::LEFT: {
			player->Left = event.State;
			break; 
		}
		case iw::RIGHT: {
			player->Right = event.State;
			break; 
		}
		case iw::X: {
			player->Dash = event.State;
			break;
		}
		case iw::R: {
			LOG_INFO << player->Up;

			if (event.State) {
				Bus->send<ResetLevelEvent>();
			}

			break;
		}
	}

	return true;
}

bool PlayerSystem::On(
	iw::ActionEvent& e)
{
	switch (e.Action) {
		case iw::val(Actions::LONG_DASH_ACTIVE): {
			Player* player = m_player.Find<Player>();

			if (e.as<LongDashEvent>().Active) {
				player->DashTime = 32.0f / 60.0f;
				player->Dash = true;
			}

			else {
				player->DashTime = playerPrefab.DashTime;
				player->Dash = false;
			}

			break;
		}
		case iw::val(Actions::TRANSITION_TO_LEVEL): {
			TransitionToLevelEvent& event = e.as<TransitionToLevelEvent>();

			Player*        p = m_player.Find<Player>();
			iw::Rigidbody* r = m_player.Find<iw::Rigidbody>();
			
			iw::vector3 previous = r->Trans().Position;

			if (event.PlayerPosition.y == 0) event.PlayerPosition.y = 1;
			
			auto player = r;

			float start = iw::TotalTime();
			float wait = 1.5f;

			Task->queue([=]() {
				while (iw::TotalTime() - start < wait) {
					player->Trans().Position = iw::lerp(
						previous,
						event.PlayerPosition,
						(iw::TotalTime() - start) / wait);
				}

				player->Trans().Position = event.PlayerPosition;
			});

			player->SetCol(nullptr);

			break;
		}
		case iw::val(Actions::START_LEVEL): {
			Player*              p = m_player.Find<Player>();
			iw::Transform*       t = m_player.Find<iw::Transform>();
			iw::Rigidbody*       r = m_player.Find<iw::Rigidbody>();
			iw::SphereCollider*  s = m_player.Find<iw::SphereCollider>();

			bool up    = p->Up;
			bool down  = p->Down;
			bool left  = p->Left;
			bool right = p->Right;

			*p = playerPrefab;

			p->Up    = up;
			p->Down  = down;
			p->Left  = left;
			p->Right = right;

			t->Position = e.as<StartLevelEvent>().PlayerPosition;
			t->Scale = 0.75f;

			r->SetCol(s);
			r->SetTrans(t);
			r->SetIsLocked(iw::vector3(0, 1, 0));

			r->SetSimGravity(0);
			r->SetVelocity(0);
			r->SetForce(0);

			p->Transition = false;

			// no break
		}
		case iw::val(Actions::RESET_LEVEL): {
			m_playerModel->GetMesh(0).Material()->Set("baseColor", iw::vector4(0.8f, 1.0f));
			m_playerModel->GetMesh(0).Material()->Set("reflectance", 1.0f);
			m_playerModel->GetMesh(0).Material()->Set("refractive", 1.0f);
			break;
		}
		case iw::val(Actions::GAME_STATE): {
			GameStateEvent& event = e.as<GameStateEvent>();
			if (event.State == SOFT_RUN) {
				m_player.Find<Player>()->Speed = 4.5f;
			}

			else if (event.State == SOFT_PAUSE) {
				m_player.Find<Player>()->Speed = 0.0f;
				m_player.Find<iw::Rigidbody>()->SetVelocity(0);
			}

			break;
		}
	}

	return false;
}

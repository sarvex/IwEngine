#include "Systems/Player_System.h"

int PlayerSystem::Initialize()
{
	player = sand->MakeTile<iw::Circle, Player, Armorments>(A_texture_player, true);

	Player*        p = player.Set<Player>();
	iw::Circle*    c = player.Find<iw::Circle>();
	iw::Rigidbody* r = player.Find<iw::Rigidbody>();

	c->Radius = 4;

	auto [w, h] = sand->GetSandTexSize2();
	r->Transform.Position = glm::vec3(w, h, 0);
	r->SetMass(10);

	Armorments* guns = player.Set<Armorments>(WeaponType::CANNON);
	guns->Weapons.emplace(WeaponType::CANNON,      MakeCannonInfo());
	guns->Weapons.emplace(WeaponType::MINIGUN,     MakeMinigunInfo());
	guns->Weapons.emplace(WeaponType::SUPER_LASER, MakeMinigunInfo());

	return 0;
}

void PlayerSystem::FixedUpdate()
{
	Player*        p = player.Find<Player>();
	iw::Rigidbody* r = player.Find<iw::Rigidbody>();
	iw::Transform& t = r->Transform;

	glm::vec3 up  = -t.Up();
	glm::vec3 right = glm::vec3(-up.y, up.x, 0);
	glm::vec3 left = -right;

	p->i_up    = iw::Keyboard::KeyDown(iw::W);
	p->i_down  = iw::Keyboard::KeyDown(iw::S);
	p->i_left  = iw::Keyboard::KeyDown(iw::A);
	p->i_right = iw::Keyboard::KeyDown(iw::D);
	p->i_fire1 = iw::Mouse::ButtonDown(iw::LMOUSE);
	p->i_fire2 = iw::Mouse::ButtonDown(iw::RMOUSE);

	r->Velocity.x = 0;
	r->Velocity.y = 0;

	int borderFar = 375;
	int borderNear = 25;

	bool atTop    = t.Position.y > borderFar;
	bool atBottom = t.Position.y < borderNear;
	bool atRight  = t.Position.x > borderFar;
	bool atLeft   = t.Position.x < borderNear;

	float speed = 150;

	if (p->i_up)    r->Velocity.y = atTop    ? 0 :  speed; // todo: make this slow stop
	if (p->i_down)  r->Velocity.y = atBottom ? 0 : -speed;
	if (p->i_right) r->Velocity.x = atRight  ? 0 :  speed;
	if (p->i_left)  r->Velocity.x = atLeft   ? 0 : -speed;
}

void PlayerSystem::Update()
{
	Player*        p = player.Find<Player>();
	iw::Transform* t = player.Find<iw::Transform>();

	p->timer.Tick();

	if (   p->i_fire1 
		&& p->timer.Can("fire1"))
	{
		auto [x, y, dx, dy] = GetShot(t->Position.x, t->Position.y, sand->sP.x, sand->sP.y, 1250/4, 10, 2);

		float speed = sqrt(dx*dx+dy*dy);
		dx += iw::randf() * speed * .05f;
		dy += iw::randf() * speed * .05f;
		
		Bus->push<SpawnProjectile_Event>(x, y, dx, dy, SpawnProjectile_Event::BULLET);
	}
	
	if (   p->i_fire2 
		&& p->timer.Can("fire2")
		&& p->can_fire_laser)
	{
		auto [x, y, dx, dy] = GetShot(t->Position.x, t->Position.y, sand->sP.x, sand->sP.y, 1800 + iw::randf() * 400, 10, 7);

		//dx += iw::randf() * 100;
		//dy += iw::randf() * 100;
		
		Bus->push<SpawnProjectile_Event>(x, y, dx, dy, SpawnProjectile_Event::LASER);
		Bus->push<ChangeLaserFluid_Event>(-1);
	}
}

bool PlayerSystem::On(iw::ActionEvent& e)
{
	switch (e.Action) {
		case HEAL_PLAYER: 
		{
			player.Find<iw::Tile>()->ReinstateRandomPixel();
			break;
		}
	}

	return false;
}

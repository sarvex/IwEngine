#include "Core/EntryPoint.h"
#include "space/space.h"

#include <iostream>

struct Position { int x, y;  };
struct Velocity { int x, y;  };
struct Mesh     { int verts; };
struct Collider { int verts; };
struct Player   {};
struct Bullet   {};
struct Asteroid { int level; };
struct Score    { int score; };

void physics_update(iwent::view<Position, Velocity, Collider>& view) {
	
}

class Game : public IwEngine::Application {
public:
	void Run() override {
		iwent::space<Game> space;
		space.make_common<Position>();
		space.make_subspace<Velocity, Collider, Player, Asteroid>();
		space.make_subspace<Mesh>();

		space.make_action<Position, Velocity, Collider>(physics_update);

		auto e = space.create();
		space.assign<Position>(e, Position{ 1, 2 });
		space.assign<Velocity>(e, Velocity{ 1, 2 });
		space.assign<Collider>(e, Collider{ 5 });
		space.assign<Mesh>    (e, Mesh{ 5 });

		std::cin.get();
	}
};

IwEngine::Application* CreateApplication() {
	return new Game();
}

//iwm	iwmath
//iwecs	iwecs
//iwevt	iwevents
//iwi	iwinput
//iwe	iwengine
//iwp	iwphysics
//iwa	iwaudio

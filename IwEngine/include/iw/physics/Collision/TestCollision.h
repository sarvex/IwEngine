#pragma once

#include "iw/physics/Collision/Collider.h"
#include "iw/physics/Collision/ManifoldPoints.h"

namespace iw {
namespace Physics {
	// x = col impl
	// o = col no impl
	// _ = no col
	//
	//         | Sphere  | Capsule | Plane  | Hull | Mesh
	// Sphere  |    x    |    x    |    x   |   x  |  o
	// Capsule |         |    o    |    o   |   o  |  o
	// Plane   |         |         |    _   |   o  |  o
	// Hull    |         |         |        |   x  |  o
	// Mesh    |         |         |        |      |  _?
	ManifoldPoints TestCollision(
		const Collider* a, const Transform* at,
		const Collider* b, const Transform* bt);
}

	using namespace Physics;
}

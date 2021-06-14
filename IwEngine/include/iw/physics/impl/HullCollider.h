#pragma once

#include "Collider.h"
#include <vector>
#include <type_traits>

namespace iw {
namespace Physics {
namespace impl {
	template<
		Dimension _d>
	struct HullCollider
		: Collider<_d>
	{
		using vec_t = _vec<_d>;
		using aabb_t = AABB<_d>;

		std::vector<vec_t> m_points;

		HullCollider(
			const std::vector<vec_t>& points = {},
			bool isMesh = false // this is bad
		)
			: Collider<_d>(isMesh ? ColliderType::MESH : ColliderType::HULL)
			, m_points(points)
		{}

		HullCollider& operator=(
			const HullCollider & copy)
		{
			SetPoints(copy.m_points); // dont copy isMesh?
			return *this;
		}

		void SetPoints(
			const std::vector<vec_t>& points)
		{
			m_points = points;
			m_boundsOutdated = true;
		}

		void AddPoint(
			const vec_t& p)
		{
			m_points.push_back(p);
			m_boundsOutdated = true;
		}

		void RemovePoint(
			const vec_t& p)
		{
			m_points.erase(
				std::find(m_points.begin(), m_points.end(), p)
			);

			m_boundsOutdated = true;
		}

		vec_t FindFurthestPoint(
			const Transform* transform,
			const vec_t&     direction) const override
		{
			//glm::vec4 dir = glm::vec4(direction, 1);
			//dir *= transform->WorldTransformation().inverted(); // I think we can transform the direction instead of the collider here

			//direction = dir.xyz();

			vec_t maxPoint = vec_t(0);
			float maxDistance = -FLT_MAX;

			for (const vec_t& point : m_points) {

				glm::vec4 v;

				if constexpr (_d == d2) v = glm::vec4(point, 0, 1);
				else                    v = glm::vec4(point,    1);

				vec_t p = vec_t(v * glm::transpose(transform->WorldTransformation())); // see ^^

				float distance = glm::dot(p, direction);
				if (distance > maxDistance) {
					maxDistance = distance;
					maxPoint    = p;
				}
			}

			//glm::vec4 mp = glm::vec4(maxPoint, 1);  // and then transform the final point?
			//mp *= transform->WorldTransformation();

			return maxPoint;//mp.xyz();


			//if (m_points.size() == 0) return vec_t(0);

			//_vec<d3> d;
			//if constexpr (_d == d2) d = _vec<d3>(direction, 0);
			//else                    d = direction;

			//vec_t dir = (vec_t)(d * transform->WorldRotation());

			//vec_t maxPoint;
			//float maxDistance = FLT_MIN;

			//for (vec_t point : m_points) {
			//	float distance = glm::dot(point, dir);
			//	if (distance > maxDistance) {
			//		maxDistance = distance;
			//		maxPoint    = point;
			//	}
			//}

			//glm::vec4 v;

			//if constexpr (_d == d2) v = glm::vec4(maxPoint, 0, 1);
			//else                    v = glm::vec4(maxPoint,    1);

			//return vec_t(v * transform->WorldTransformation());
		}
	protected:
		aabb_t GenerateBounds() const override {
			aabb_t bounds;

			bounds.Min = vec_t(FLT_MAX);
			bounds.Max = vec_t(FLT_MIN);

			for (const vec_t& v : m_points) {
				for (size_t i = 0; i < _d; i++)
				{
					if (v[i] < bounds.Min[i]) bounds.Min[i] = v[i];
					if (v[i] > bounds.Max[i]) bounds.Max[i] = v[i];
				}
			}

			return bounds;
		}
	};
}

	inline impl::HullCollider<d3> MakeCubeCollider() {
		impl::HullCollider<d3> collider;
		collider.AddPoint(glm::vec3(-1, -1, -1)); // 0 
		collider.AddPoint(glm::vec3(-1,  1, -1)); // 1 
		collider.AddPoint(glm::vec3( 1,  1, -1)); // 2 
		collider.AddPoint(glm::vec3( 1, -1, -1)); // 3 

		collider.AddPoint(glm::vec3(-1, -1,  1)); // 7
		collider.AddPoint(glm::vec3( 1, -1,  1)); // 6
		collider.AddPoint(glm::vec3( 1,  1,  1)); // 5
		collider.AddPoint(glm::vec3(-1,  1,  1)); // 4

		collider.Bounds();

		return collider;
	}

	inline impl::HullCollider<d3> MakeTetrahedronCollider() {
		impl::HullCollider<d3> collider;
		collider.AddPoint(glm::vec3(cos(Pi2 * 0 / 3), -1, sin(Pi2 * 0 / 3)));
		collider.AddPoint(glm::vec3(cos(Pi2 * 1 / 3), -1, sin(Pi2 * 1 / 3)));
		collider.AddPoint(glm::vec3(cos(Pi2 * 2 / 3), -1, sin(Pi2 * 2 / 3)));
		collider.AddPoint(glm::vec3(0, 1, 0));

		collider.Bounds();

		return collider;
	}

	inline impl::HullCollider<d2> MakeSquareCollider() {
		impl::HullCollider<d2> collider;
		collider.AddPoint(glm::vec2(-1,  1)); // 0 
		collider.AddPoint(glm::vec2(-1, -1)); // 1 
		collider.AddPoint(glm::vec2( 1, -1)); // 2 
		collider.AddPoint(glm::vec2( 1,  1)); // 3 

		collider.Bounds();

		return collider;
	}

	inline impl::HullCollider<d2> MakeTriangleCollider() {
		impl::HullCollider<d2> collider;
		collider.AddPoint(glm::vec2(cos(Pi2 * 0 / 3), sin(Pi2 * 0 / 3)));
		collider.AddPoint(glm::vec2(cos(Pi2 * 1 / 3), sin(Pi2 * 1 / 3)));
		collider.AddPoint(glm::vec2(cos(Pi2 * 2 / 3), sin(Pi2 * 2 / 3)));

		collider.Bounds();

		return collider;
	}
}

	using namespace Physics;
}

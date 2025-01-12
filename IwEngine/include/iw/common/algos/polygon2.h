#pragma once

#include "iw/common/algos/geom2.h"
#include <array>
#include <vector>
#include <utility>
#include <algorithm>
#include <functional>

namespace iw {
namespace common {
	using polygon_cut = std::tuple<
		std::vector<glm::vec2>, std::vector<unsigned>,
		std::vector<glm::vec2>, std::vector<unsigned>>;

	using polygon_crack = std::vector<
		std::pair<std::vector<glm::vec2>, std::vector<unsigned>>>;

	using c_aabb2 = std::pair<glm::vec2, glm::vec2>;
	using c_aabb  = std::pair<glm::vec3, glm::vec3>;

	template<
		typename _t>
	std::vector<std::vector<glm::vec2>> MakePolygonFromField(
		_t* field,
		size_t width, size_t height,
		const _t& threshold)
	{
		return MakePolygonFromField(
			field,
			width, height,
			[&](const _t& x)
			{
				return x >= threshold;
			}
		);
	}

	template<
		typename _t>
	std::vector<std::vector<glm::vec2>> MakePolygonFromField(
		_t* field,
		size_t width, size_t height,
		std::function<bool(const _t&)> test)
	{
		bool* f = new bool[width * height];

		for (size_t i = 0; i < width * height; i++)
		{
			f[i] = test(field[i]);
		}

		auto out = MakePolygonFromField(f, width, height);

		delete[] f;

		return out;
	}

	inline
	std::tuple<
		glm::vec2&,
		glm::vec2&,
		glm::vec2&>
	GetTriangle(
		std::vector<glm::vec2>& polygon,
		std::vector<unsigned>& index,
		size_t triangle)
	{
		glm::vec2& a = polygon[index[triangle    ]];
		glm::vec2& b = polygon[index[triangle + 1]];
		glm::vec2& c = polygon[index[triangle + 2]];

		return { a, b, c };
	}

	// calls __container {vec2, vec2, vec2, vec2};
	template<typename C, typename T = std::decay_t<decltype(*begin(std::declval<C>()))>, typename = std::enable_if_t<std::is_same_v<T, glm::vec2>>>
	C MakePolygonFromBounds(
		const c_aabb2& bounds)
	{
		const auto& [min, max] = bounds;

		return C {
			min,
			glm::vec2(max.x, min.y),
			max,
			glm::vec2(min.x, max.y)
		};
	}

	template<typename C, typename T = std::decay_t<decltype(*begin(std::declval<C>()))>>
	void TransformPolygon(
		C& polygon,
		Transform* transform)
	{
		auto begin = polygon.begin();
		auto end   = polygon.end();
		for (T& vert : polygon)
		{
			vert = TransformPoint<Dimension::d2>(vert, transform);
		}
	}

	template<typename C, typename T = std::decay_t<decltype(*begin(std::declval<C>()))>, typename = std::enable_if_t<std::is_same_v<T, glm::vec2>>>
	c_aabb2 GenPolygonBounds(
		const C& polygon)
	{
		c_aabb2 bounds(glm::vec2(FLT_MAX), glm::vec2(-FLT_MAX));
		auto& [min, max] = bounds;

		for (const glm::vec2& v : polygon)
		{
			if (v.x > max.x) max.x = v.x;
			if (v.x < min.x) min.x = v.x;
			if (v.y > max.y) max.y = v.y;
			if (v.y < min.y) min.y = v.y;
		}

		return bounds;
	}

	template<typename C, typename T = std::decay_t<decltype(*begin(std::declval<C>()))>, typename = std::enable_if_t<std::is_same_v<T, glm::vec3>>>
	c_aabb GenPolygonBounds(
		const C& polygon)
	{
		c_aabb bounds(glm::vec3(FLT_MAX), glm::vec3(-FLT_MAX));
		auto& [min, max] = bounds;

		for (const glm::vec3& v : polygon)
		{
			if (v.x > max.x) max.x = v.x;
			if (v.x < min.x) min.x = v.x;
			if (v.y > max.y) max.y = v.y;
			if (v.y < min.y) min.y = v.y;
			if (v.z > max.z) max.z = v.z;
			if (v.z < min.z) min.z = v.z;
		}

		return bounds;
	}

	IWCOMMON_API
	std::vector<std::vector<glm::vec2>> MakePolygonFromField(
		bool* field,
		size_t width, size_t height);

	IWCOMMON_API
	std::vector<unsigned> TriangulatePolygon(
		const std::vector<glm::vec2>& polygon);

	//IWCOMMON_API
	//std::vector<unsigned> TriangulateSweep(
	//	std::vector<glm::vec2>& verts);

	IWCOMMON_API
	void RemoveTinyTriangles(
		std::vector<glm::vec2>& polygon,
		std::vector<unsigned>&  index,
		float ratioOfAreaToRemove = .001);

	IWCOMMON_API
	polygon_cut CutPolygon(
		const std::vector<glm::vec2>& verts,
		const std::vector<unsigned>& index,
		glm::vec2 lineA, glm::vec2 lineB);

	IWCOMMON_API
	polygon_crack CrackPolygon(
		const std::vector<glm::vec2>& verts);

	IWCOMMON_API
	c_aabb2 TransformBounds(
		const c_aabb2& bounds,
		Transform* transform);

	//IWCOMMON_API
	inline c_aabb TransformBounds(
		const c_aabb& bounds,
		const Transform* transform) { return c_aabb(); } // needs impl, should split file into polygon and polygon2 or have impl structure like physics

	//IWCOMMON_API
	//c_aabb2 GenPolygonBounds(
	//	const std::vector<glm::vec2>& polygon);

	//// this should go into a polygon.h file
	//IWCOMMON_API
	//c_aabb GenPolygonBounds(
	//	const std::vector<glm::vec3>& polygon);

	IWCOMMON_API
	c_aabb2 GenTriangleBounds(
		const glm::vec2& v1,
		const glm::vec2& v2,
		const glm::vec2& v3);

	IWCOMMON_API
	void AddPointToPolygon(
		std::vector<glm::vec2>& verts,
		std::vector<unsigned>& indices,
		glm::vec2 point);
}

	using namespace common;
}

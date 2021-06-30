#include "iw/common/algos/geom2.h"

namespace iw {
namespace common {
	bool IsClockwise(
		glm::vec2 a, glm::vec2 b, glm::vec2 c) {
		return cross_length(b - a, c - a) < 0; // this was backwards before idk how that worked?
	}

	bool HasPoint(
		glm::vec2 a, glm::vec2 b, glm::vec2 c,
		glm::vec2 p) 
	{
		if ((a.x - b.x) / (a.y - b.y) == (a.x - c.x) / (a.y - c.y)) {
			return true; // points are on a line
		}

		glm::vec2 v = p;
		glm::vec2 v0 = a;
		glm::vec2 v1 = b - a;
		glm::vec2 v2 = c - a;

		float ca =  (cross_length(v, v2) - cross_length(v0, v2)) / cross_length(v1, v2);
		float cb = -(cross_length(v, v1) - cross_length(v0, v1)) / cross_length(v1, v2);

		return ca > 0 && cb > 0 && ca + cb < 1;
	}

	glm::vec2 LineIntersection(
		glm::vec2 a0, glm::vec2 a1,
		glm::vec2 b0, glm::vec2 b1)
	{
		glm::vec2 a0_a1 = a1 - a0;
		glm::vec2 b0_b1 = b1 - b0;
		glm::vec2 b0_a0 = a0 - b0;

		return a0 + a0_a1
			* cross_length(b0_b1, b0_a0)
			/ cross_length(a0_a1, b0_b1);
	}

	std::pair<glm::vec2, bool> SegmentIntersection(
		glm::vec2 a0, glm::vec2 a1,
		glm::vec2 b0, glm::vec2 b1)
	{
		glm::vec2 a0_a1 = a1 - a0;
		glm::vec2 b0_b1 = b1 - b0;
		glm::vec2 b0_a0 = a0 - b0;

		float x = cross_length(b0_b1, b0_a0)
			    / cross_length(a0_a1, b0_b1);

		return { a0 + a0_a1 * x, x >= 0 && x <= 1 };
	}

	float TriangleArea(
		const glm::vec2& a, 
		const glm::vec2& b, 
		const glm::vec2& c)
	{
		return cross_length(b - a, c - a) * 0.5f;
	}

	std::pair<glm::vec2, float> TriangleCircle(
		const glm::vec2& a,
		const glm::vec2& b,
		const glm::vec2& c)
	{
		float ab = glm::length2(a);
		float cd = glm::length2(b);
		float ef = glm::length2(c);

		float ax = a.x;
		float ay = a.y;
		float bx = b.x;
		float by = b.y;
		float cx = c.x;
		float cy = c.y;

		float circum_x = (ab * (cy - by) + cd * (ay - cy) + ef * (by - ay)) / (ax * (cy - by) + bx * (ay - cy) + cx * (by - ay));
		float circum_y = (ab * (cx - bx) + cd * (ax - cx) + ef * (bx - ax)) / (ay * (cx - bx) + by * (ax - cx) + cy * (bx - ax));

		glm::vec2 p(circum_x / 2, circum_y / 2);
		float r = glm::distance2(a, p);

		return { p, r };
	}

	bool CircleHasPoint(
		const std::pair<glm::vec2, float>& circle, 
		const glm::vec2& x)
	{
		return glm::distance2(x, circle.first) <= circle.second * circle.second;
	}
}
}

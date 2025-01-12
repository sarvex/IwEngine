#include "iw/Sand/Tile.h"

IW_PLUGIN_SAND_BEGIN

Tile::Tile(
	const Texture& texture)
{
	m_sprite = texture;

	if (!m_sprite.m_colors) {
		m_sprite.SetFilter(iw::NEAREST);
		m_sprite.CreateColors();
	}

	m_bounds = AABB2(glm::vec2(0.f), m_sprite.Dimensions() /*- glm::vec2(1.f)*/);

	m_polygon = MakePolygonFromBounds<std::vector<glm::vec2>>(m_bounds);
	m_index   = TriangulatePolygon(m_polygon);

	m_uv = m_polygon; // uv origin is 0

	glm::vec2 halfSize = m_sprite.Dimensions() / 2.0f;

	for (glm::vec2& v : m_collider) v -= halfSize; // Origins to middle
	for (glm::vec2& v : m_polygon)  v -= halfSize;

	m_bounds.Min -= halfSize; // could make better flow
	m_bounds.Max -= halfSize;

	// Set cell counts/health, could maybe do this 
	// in the UpdateCeooliderPolygon function in the MakeFromField lambda...

	for (int i = 0; i < m_sprite.ColorCount32(); i++)
	{
		PixelState state = State(i);
		
		if (state == EMPTY) 
		{
			continue;
		}

		if (state == REMOVED)
		{
			m_removedCells.push_back(i);
		}

		else {
			m_currentCells.push_back(i);
			m_currentCellCount++;
		}

		m_initalCellCount++;
	}
}

void Tile::UpdateColliderPolygon()
{
	unsigned* colors = m_sprite.Colors32();
	glm::vec2 size   = m_sprite.Dimensions();

	auto colliders = MakePolygonFromField<unsigned>( // why does this NEED template arg
		colors, 
		size.x, size.y,
		[&](const unsigned& color)
		{
			return cState(color) > REMOVED; // i wonder if this optimizes for only the alpha channel?
		});
	
	if (colliders.size() == 0) return;

	size_t vertCount = 0;
	size_t idnxCount = 0;

	for (auto& chain : colliders)
	{
		vertCount += chain.size();
	}

	m_collider     .clear();
	m_colliderIndex.clear();

	m_collider     .reserve(    vertCount);
	m_colliderIndex.reserve(3 * vertCount); // approx, find avg number of tris from math

	unsigned lastSize = 0;

	for (auto& chain : colliders)
	{
		std::vector<unsigned> tris = TriangulatePolygon(chain);
		for (unsigned& index : tris)
		{
			index += lastSize;
		}

		m_collider     .insert(m_collider     .end(), chain.begin(), chain.end());
		m_colliderIndex.insert(m_colliderIndex.end(), tris .begin(), tris .end());

		lastSize = (unsigned)m_collider.size();
	}

	//m_collider      = colliders[0]; // should combine into single polygon?
	//m_colliderIndex = TriangulatePolygon(colliders[0]);

	glm::vec2 halfSize = m_sprite.Dimensions() / 2.0f;
	for (glm::vec2& v : m_collider) v -= halfSize; // Origins to middle ?

	// should combine same verts
	// should also simplying too many tris!!
	RemoveTinyTriangles(m_collider, m_colliderIndex, 0.001f); // find best value for this, colliders can be less detailed
}

IW_PLUGIN_SAND_END

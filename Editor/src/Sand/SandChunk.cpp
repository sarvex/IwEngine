#include "Sand/SandChunk.h"
#include <algorithm>
#include <iw/log/logger.h>

SandChunk::SandChunk()
	: m_cells(nullptr)
	, m_x(0)
	, m_y(0)
	, m_width (0)
	, m_height(0)
	, m_filledCellCount(0)
	, m_lastTick(0)
{
	ResetRect();
}

SandChunk::SandChunk(
	Cell* cells, 
	WorldCoord x,     WorldCoord y,
	ChunkCoord width, ChunkCoord height,
	Tick currentTick)
	: m_cells(cells)
	, m_x(x)
	, m_y(y) 
	, m_width (width)
	, m_height(height)
	, m_filledCellCount(0)
	, m_lastTick(currentTick)
{
	ResetRect();
}

SandChunk::SandChunk(
	const SandChunk& copy)
	: m_cells(copy.m_cells)
	, m_x(copy.m_x)
	, m_y(copy.m_y)
	, m_width (copy.m_width)
	, m_height(copy.m_height)
	, m_filledCellCount(copy.m_filledCellCount.load())
	, m_lastTick(copy.m_lastTick)
{
	ResetRect();
}

void SandChunk::SetCell(
	Index index,
	const Cell& cell,
	Tick currentTick)
{
	m_lastTick = currentTick;

	std::unique_lock lock(m_changesMutex);

	SetCellData(index, cell, currentTick);
	UpdateRect(index % m_width, index / m_width);
}

void SandChunk::MoveCell(
	SandChunk* sourceChunk,
	WorldCoord x,   WorldCoord y,
	WorldCoord xTo, WorldCoord yTo)
{
	std::unique_lock lock(m_changesMutex);

	m_changes.emplace_back(
		sourceChunk,
		sourceChunk->GetIndex(x, y),
		GetIndex(xTo, yTo)
	);
}

void SandChunk::CommitMovedCells(
	Tick currentTick)
{
	//m_lastCurrentTick = currentTick;

	ResetRect();

	// remove moves that have their destinations filled

	constexpr size_t _CHUNK = 0;
	constexpr size_t _SRC   = 1;
	constexpr size_t _DEST  = 2;

	for (size_t i = 0; i < m_changes.size(); i++) {
		if (m_cells[std::get<_DEST>(m_changes[i])].Type != CellType::EMPTY) {
			m_changes[i] = m_changes.back(); m_changes.pop_back();
			i--;
		}
	}

	if (m_changes.size() == 0) return;

	// sort by destination

	std::sort(m_changes.begin(), m_changes.end(),
		[=](auto& a, auto& b) { return std::get<2>(a) < std::get<2>(b); } // cant pass _DEST (constexpr) I guess
	);

	// pick random source for each destination

	size_t ip = 0;

	m_changes.emplace_back(nullptr, -1, -1); // to catch last change

	for (size_t i = 0; i < m_changes.size() - 1; i++) {
		if (std::get<_DEST>(m_changes[i + 1]) != std::get<_DEST>(m_changes[i])) {
			auto [sourceChunk, src, dest] = m_changes[ip + iw::randi(i - ip)];

			SetCellData(dest, sourceChunk->GetCell(src), currentTick);
			sourceChunk->SetCell(src, Cell::GetDefault(CellType::EMPTY), currentTick);

			ip = i + 1;
		}
	}

	m_changes.clear();
}

// Private

void SandChunk::ResetRect() {
	m_minX = m_width;
	m_minY = m_height;
	m_maxX = -1;
	m_maxY = -1;
}

void SandChunk::UpdateRect(
	WorldCoord x, WorldCoord y)
{
	if (x <= m_minX) m_minX = iw::clamp<WorldCoord>(x - 2, 0, m_width);
	if (x >= m_maxX) m_maxX = iw::clamp<WorldCoord>(x + 2, 0, m_width);

	if (y <= m_minY) m_minY = iw::clamp<WorldCoord>(y - 2, 0, m_height);
	if (y >= m_maxY) m_maxY = iw::clamp<WorldCoord>(y + 2, 0, m_height);
}

void SandChunk::SetCellData(
	Index index,
	const Cell& cell,
	Tick currentTick)
{
	Cell& dest = m_cells[index];

	if (   cell.Type == CellType::EMPTY
		&& dest.Type != CellType::EMPTY)
	{
		--m_filledCellCount;
	}
		
	else if (  cell.Type != CellType::EMPTY
			&& dest.Type == CellType::EMPTY)
	{
		++m_filledCellCount;
	}

	dest = cell;
	dest.LastUpdateTick = currentTick;
}

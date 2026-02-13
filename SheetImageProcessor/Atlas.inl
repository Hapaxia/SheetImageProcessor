//////////////////////////////////////////////////////////////////////////////
//
// Sheet Image Processor (https://github.com/Hapaxia/SheetImageProcessor
// --
//
// Atlas
//
// Copyright(c) 2025-2026 M.J.Silk
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions :
//
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software.If you use this software
// in a product, an acknowledgment in the product documentation would be
// appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
// M.J.Silk
// MJSilk2@gmail.com
//
//////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Atlas.hpp"

#include <algorithm>

#include <iostream>

namespace sheetimageprocessor
{

inline Atlas::Atlas()
	: m_maxSize{ 0u, 0u }
	, m_tiles{}
{

}

inline Atlas::Atlas(const Atlas& other)
{
	*this = other;
}

inline Atlas& Atlas::operator=(const Atlas& other)
{
	m_maxSize = other.m_maxSize;
	m_tiles = other.m_tiles;
	return *this;
}

inline void Atlas::clear()
{
	m_tiles.clear();
}

inline void Atlas::reserve(const std::size_t reserveAtlasSize)
{
	m_tiles.reserve(reserveAtlasSize);
}

inline void Atlas::resize(const std::size_t atlasSize, const bool resetAllTiles)
{
	const std::size_t origSize{ m_tiles.size() };
	m_tiles.resize(atlasSize, Tile{});
	if (!resetAllTiles)
		return;
	for (std::size_t i{ 0u }; i < origSize; ++i)
		m_tiles[i] = Tile{};
}

inline std::size_t Atlas::getSize() const
{
	return m_tiles.size();
}

inline void Atlas::setMaxSize(const Xy maxSize)
{
	if ((maxSize.x == 0u) || (maxSize.y == 0u))
		m_maxSize = { 0u, 0u };
	else
		m_maxSize = maxSize;
}

inline Xy Atlas::getMaxSize() const
{
	return m_maxSize;
}

inline Xy Atlas::getMaxUsed() const
{
	Xy maxUsed{ 0u, 0u };
	for (const auto& tile : m_tiles)
	{
		const Xy maxRectCorner{ tile.rect.position + tile.rect.size };
		if (maxRectCorner.x > maxUsed.x)
			maxUsed.x = maxRectCorner.x;
		if (maxRectCorner.y > maxUsed.y)
			maxUsed.y = maxRectCorner.y;
	}
	return maxUsed;
}

inline void Atlas::scale(const float factor)
{
	scale(factor, factor);
}

inline void Atlas::scale(const float factorX, const float factorY)
{
	for (auto& tile : m_tiles)
	{
		tile.rect.position.x = static_cast<std::size_t>(std::lround(std::floor(tile.rect.position.x * factorX)));
		tile.rect.position.y = static_cast<std::size_t>(std::lround(std::floor(tile.rect.position.y * factorY)));
		tile.rect.size.x = static_cast<std::size_t>(std::lround(std::floor(tile.rect.size.x * factorX)));
		tile.rect.size.y = static_cast<std::size_t>(std::lround(std::floor(tile.rect.size.y * factorY)));
		tile.offset.x = static_cast<std::size_t>(std::lround(std::floor(tile.offset.x * factorX)));
		tile.offset.y = static_cast<std::size_t>(std::lround(std::floor(tile.offset.y * factorY)));
		tile.anchor.x = static_cast<std::size_t>(std::lround(std::floor(tile.anchor.x * factorX)));
		tile.anchor.y = static_cast<std::size_t>(std::lround(std::floor(tile.anchor.y * factorY)));
	}
}

inline void Atlas::offsetAllPositions(const Xy offset)
{
	for (auto& tile : m_tiles)
		tile.rect.position += offset;
}

inline void Atlas::add(const Atlas& other, const bool allowMaxSizeIncrease)
{
	if (this == &other)
		return;

	m_tiles.insert(m_tiles.end(), other.m_tiles.begin(), other.m_tiles.end());
	if (allowMaxSizeIncrease)
	{
		if ((m_maxSize.x == 0u) || (m_maxSize.y == 0u) || (other.m_maxSize.x == 0u) || (other.m_maxSize.y == 0u))
			m_maxSize = { 0u, 0u };
		else
		{
			m_maxSize.x = std::max(m_maxSize.x, other.m_maxSize.x);
			m_maxSize.y = std::max(m_maxSize.y, other.m_maxSize.y);
		}
	}
}

inline void Atlas::add(const Tile& tile)
{
	if (!priv_isWithinMaxSize(tile.rect))
		return;
	m_tiles.push_back(tile);
}

inline void Atlas::set(const std::size_t index, const Tile& tile)
{
	assert(priv_isValidIndex(index));
	if (!priv_isValidIndex(index))
		return;
	if (!priv_isWithinMaxSize(tile.rect))
		return;
	m_tiles[index] = tile;
}

inline Atlas::Tile Atlas::get(const std::size_t index) const
{
	assert(priv_isValidIndex(index));
	if (!priv_isValidIndex(index))
		return {};
	return m_tiles[index];
}

inline Atlas::Tile& Atlas::access(const std::size_t index)
{
	assert(priv_isValidIndex(index));
	if (!priv_isValidIndex(index))
		throw Exception("Cannot access tile: invalid index.");

	return m_tiles[index];
}

inline void Atlas::setById(const Tile& tile, const bool allowAddIfUnique)
{
	auto it{ std::find_if(m_tiles.begin(), m_tiles.end(), [&](const Tile& t) { return (t.id == tile.id); }) };

	if (it != m_tiles.end())
	{
		if (!priv_isWithinMaxSize(tile.rect))
			return;
		*it = tile;
	}
	else if (allowAddIfUnique)
		add(tile);
}

inline Atlas::Tile Atlas::getById(const std::size_t id) const
{
	auto it{ std::find_if(m_tiles.begin(), m_tiles.end(), [&](const Tile& tile) { return (tile.id == id); }) };
	if (it == m_tiles.end())
		return {};
	return *it;
}

inline void Atlas::setRect(const std::size_t index, const Rect rect)
{
	assert(priv_isValidIndex(index));
	if (!priv_isValidIndex(index))
		return;
	if (!priv_isWithinMaxSize(rect))
		return;
	m_tiles[index].rect = rect;
}

inline void Atlas::setOffset(const std::size_t index, const Xy offset)
{
	assert(priv_isValidIndex(index));
	if (!priv_isValidIndex(index))
		return;
	m_tiles[index].offset = offset;
}

inline void Atlas::setAnchor(const std::size_t index, const Xy anchor)
{
	assert(priv_isValidIndex(index));
	if (!priv_isValidIndex(index))
		return;
	m_tiles[index].anchor = anchor;
}

inline void Atlas::setId(const std::size_t index, const std::size_t id)
{
	assert(priv_isValidIndex(index));
	if (!priv_isValidIndex(index))
		return;
	m_tiles[index].id = id;
}

inline void Atlas::setCategory(const std::size_t index, const std::size_t category)
{
	assert(priv_isValidIndex(index));
	if (!priv_isValidIndex(index))
		return;
	m_tiles[index].category = category;
}

inline void Atlas::setIsRotated(const std::size_t index, const bool isRotated)
{
	assert(priv_isValidIndex(index));
	if (!priv_isValidIndex(index))
		return;
	m_tiles[index].isRotated = isRotated;
}

inline void Atlas::setIsFlippedX(const std::size_t index, const bool isFlippedX)
{
	assert(priv_isValidIndex(index));
	if (!priv_isValidIndex(index))
		return;
	m_tiles[index].isFlippedX = isFlippedX;
}

inline void Atlas::setIsFlippedY(const std::size_t index, const bool isFlippedY)
{
	assert(priv_isValidIndex(index));
	if (!priv_isValidIndex(index))
		return;
	m_tiles[index].isFlippedY = isFlippedY;
}

inline void Atlas::generateFromGrid(
	const Xy startPosition,
	const Xy gridSize,
	const Xy tileSize,
	const Xy separation,
	const std::size_t category,
	const std::size_t initId)
{
	resize(gridSize.x * gridSize.y);
	Tile tile{};
	tile.rect.size = tileSize;
	tile.category = category;
	for (std::size_t y{ 0u }; y < gridSize.y; ++y)
	{
		for (std::size_t x{ 0u }; x < gridSize.x; ++x)
		{
			const std::size_t index{ (y * gridSize.x) + x };
			tile.id = initId + index;
			tile.rect.position = { startPosition.x + (x * (tileSize.x + separation.x)), startPosition.y + (y * (tileSize.y + separation.y)) };
			set(index, tile);
		}
	}
}

inline std::vector<std::size_t> Atlas::pack(
	const Xy targetRectSize,
	const std::size_t separation,
	const std::size_t expansion,
	const bool sortByArea)
{
	std::vector<std::size_t> unusedIndices{};

	const std::size_t numOfTiles{ getSize() };

	std::vector<std::size_t> orderedIndices(numOfTiles);
	for (std::size_t i{ 0u }; i < numOfTiles; ++i)
		orderedIndices[i] = i;

	const Atlas& thisAtlas{ *this };

	if (sortByArea)
		std::sort(orderedIndices.begin(), orderedIndices.end(), [&thisAtlas](const std::size_t lhs, const std::size_t rhs) { return (thisAtlas.get(lhs).rect.getArea() > thisAtlas.get(rhs).rect.getArea()); });

	struct Node
	{
		std::array<Node*, 2u> children{ nullptr, nullptr };
		Rect rect{};
		int imageIndex{ -1 };

		Node* insert(const Atlas::Tile& tile)
		{
			if ((children[0u] != nullptr) || (children[1u] != nullptr))
			{
				Node* newNode = children[0u]->insert(tile);
				if (newNode != nullptr)
					return newNode;
				return children[1u]->insert(tile);
			}
			else
			{
				if (imageIndex != -1)
					return nullptr;

				if ((tile.rect.size.x > rect.size.x) || (tile.rect.size.y > rect.size.y))
					return nullptr;

				if ((tile.rect.size.x == rect.size.x) && (tile.rect.size.y == rect.size.y))
					return this;

				children[0u] = new Node{};
				children[1u] = new Node{};

				const Xy diff{ rect.size.x - tile.rect.size.x, rect.size.y - tile.rect.size.y };

				if (diff.x > diff.y)
				{
					children[0u]->rect = Rect{ { rect.position.x, rect.position.y }, { tile.rect.size.x , rect.size.y } };
					children[1u]->rect = Rect{ { rect.position.x + tile.rect.size.x, rect.position.y }, { rect.size.x - tile.rect.size.x, rect.size.y } };
				}
				else
				{
					children[0u]->rect = Rect{ { rect.position.x, rect.position.y }, { rect.size.x , tile.rect.size.y } };
					children[1u]->rect = Rect{ { rect.position.x, rect.position.y + tile.rect.size.y }, { rect.size.x, rect.size.y - tile.rect.size.y } };
				}

				return children[0u]->insert(tile);
			}
		}

		Node* get(const std::size_t index)
		{
			const int targetIndex{ static_cast<int>(index) };

			if (imageIndex == targetIndex)
				return this;

			//std::clog << "not the one we are looking for (imageIndex = " << imageIndex << ")\n";

			if (children[0u] == nullptr)
				return nullptr;

			Node* child1{ children[0u]->get(targetIndex) };
			if (child1 != nullptr)
				return child1;

			Node* child2{ children[1u]->get(targetIndex) };
			if (child2 != nullptr)
				return child2;

			return nullptr;
		}
	};

	Xy separationVector{ separation, separation };
	Xy expansionVector{ expansion + expansion, expansion + expansion };
	Xy extraVector{ separationVector + expansionVector };

	Node* root = new Node{};
	root->rect = { { 0u, 0u }, targetRectSize + separationVector }; // adding the separation vector here allows the bottom and right edges to ignore the separation

	for (std::size_t i{ 0u }; i < numOfTiles; ++i)
	{
		Atlas::Tile tile{ get(orderedIndices[i]) };
		tile.rect.size += extraVector;

		Node* node = root->insert(tile);
		if (node != nullptr)
			node->imageIndex = static_cast<int>(orderedIndices[i]);
		else
		{
			//throw Exception("INVALID HANDLE!");
			unusedIndices.push_back(orderedIndices[i]);
		}
	}

	for (std::size_t i{ 0u }; i < numOfTiles; ++i)
	{
		const std::size_t orderedIndex{ orderedIndices[i] };
		Node* node{ root->get(orderedIndex) };
		if (node == nullptr)
			continue;
		m_tiles[orderedIndex].rect.position = node->rect.position + Xy{ expansion, expansion };
	}

	return unusedIndices;
}

inline std::vector<Atlas::Tile> Atlas::get() const
{
	return m_tiles;
}

inline std::vector<Atlas::Tile> Atlas::getAllCategory(std::size_t category) const
{
	std::size_t count{ 0u };
	for (const auto& tile : m_tiles)
	{
		if (tile.category == category)
			++count;
	}
	if (count == 0)
		return {};

	std::vector<Tile> tiles(count);
	std::size_t index{ 0u };
	for (const auto& tile : m_tiles)
	{
		if (tile.category == category)
			tiles[index++] = tile;
	}

	return tiles;
}

inline const std::vector<Atlas::Tile>& Atlas::constAccess() const
{
	return m_tiles;
}







inline bool Atlas::priv_isValidIndex(const std::size_t index) const
{
	return index < m_tiles.size();
}

inline bool Atlas::priv_isUnlimitedMaxSize() const
{
	return (m_maxSize.x == 0u);
}

inline bool Atlas::priv_isWithinMaxSize(const Xy position) const
{
	return (priv_isUnlimitedMaxSize() || ((position.x < m_maxSize.x) && (position.y < m_maxSize.y)));
}

inline bool Atlas::priv_isWithinMaxSize(const Rect rect) const
{
	return (priv_isUnlimitedMaxSize() || (((rect.position.x + rect.size.x) <= m_maxSize.x) && ((rect.position.y + rect.size.y) <= m_maxSize.y)));
}

} // namespace sheetimageprocessor

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

#include "Common.hpp"
#include "Rect.hpp"
#include "Xy.hpp"

namespace sheetimageprocessor
{

class Atlas
{
public:
	struct Tile
	{
		Rect rect{};
		Xy offset{};
		Xy anchor{};
		std::size_t id{};
		std::size_t category{};
		bool isRotated{ false };
		bool isFlippedX{ false };
		bool isFlippedY{ false };
	};

	Atlas();
	Atlas(const Atlas& other);
	Atlas& operator=(const Atlas& other);

	void clear();
	void reserve(std::size_t reserveAtlasSize);
	void resize(std::size_t atlasSize, bool resetAllTiles = false);
	std::size_t getSize() const;

	void setMaxSize(Xy maxSize);
	Xy getMaxSize() const;

	Xy getMaxUsed() const;

	void scale(float scale);
	void scale(float scaleX, float scaleY);

	void offsetAllPositions(Xy offset);

	void add(const Atlas& other, bool allowMaxSizeIncrease = true);

	void add(const Tile& tile);
	void set(std::size_t index, const Tile& tile);
	Tile get(std::size_t index) const;
	Tile& access(std::size_t index);
	void setById(const Tile& tile, bool allowAddIfUnique = false);
	Tile getById(std::size_t id) const;

	void setRect(std::size_t index, Rect rect);
	void setOffset(std::size_t index, Xy offset);
	void setAnchor(std::size_t index, Xy anchor);
	void setId(std::size_t index, std::size_t id);
	void setCategory(std::size_t index, std::size_t category);
	void setIsRotated(std::size_t index, bool isRotated);
	void setIsFlippedX(std::size_t index, bool isFlippedX);
	void setIsFlippedY(std::size_t index, bool isFlippedY);

	void generateFromGrid(
		Xy startPosition,
		Xy gridSize,
		Xy tileSize,
		Xy separation = {},
		std::size_t category = 0u,
		std::size_t initId = 0u);

	std::vector<std::size_t> pack( // returns vector of indices of tiles from original atlas that do not fit and have not been adjusted. use a backed-up Atlas to reference these indices
		Xy targetRectSize,
		std::size_t separation = 0u,
		std::size_t expansion = 0u,
		bool sortByArea = false);

	std::vector<Tile> get() const;
	std::vector<Tile> getAllCategory(std::size_t category) const;
	const std::vector<Tile>& constAccess() const;

private:
	Xy m_maxSize;
	std::vector<Tile> m_tiles;

	bool priv_isValidIndex(const std::size_t index) const;
	bool priv_isUnlimitedMaxSize() const;
	bool priv_isWithinMaxSize(const Xy position) const;
	bool priv_isWithinMaxSize(const Rect rect) const;
};

} // namespace sheetimageprocessor
#include "Atlas.inl"

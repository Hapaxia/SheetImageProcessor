//////////////////////////////////////////////////////////////////////////////
//
// Sheet Image Processor (https://github.com/Hapaxia/SheetImageProcessor
// --
//
// Rect
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
#include "Xy.hpp"

namespace sheetimageprocessor
{

class Rect
{
public:
	Xy position;
	Xy size;

	constexpr Rect()
		: position{}
		, size{}
	{
	}

	constexpr Rect(const Xy& newPosition, const Xy& newSize)
	{
		position = newPosition;
		size = newSize;
	}

	template <class T>
	constexpr Rect(const T& otherPositionAndSize)
	{
		position = static_cast<Xy>(otherPositionAndSize.position);
		size = static_cast<Xy>(otherPositionAndSize.size);
	}

	constexpr bool contains(const Xy point) const
	{
		return ((point.x >= position.x) && (point.x < (position.x + size.x)) && (point.y >= position.y) && (point.y < (position.y + size.y)));
	}
	constexpr std::size_t getArea() const
	{
		return size.x * size.y;
	}
	constexpr Xy getBottomRight() const
	{
		return{ position.x + size.x - 1u, position.y + size.y - 1u };
	}

	constexpr bool operator==(const Rect other)
	{
		return ((position == other.position) && (size == other.size));
	}
	constexpr bool operator!=(const Rect other)
	{
		return !(*this == other);
	}

	std::string asString(const bool includeNewline = false) const
	{
		return position.asString() + ", " + size.asString(true, includeNewline);
	}
};

} // namespace sheetimageprocessor

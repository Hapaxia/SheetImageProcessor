//////////////////////////////////////////////////////////////////////////////
//
// Sheet Image Processor (https://github.com/Hapaxia/SheetImageProcessor
// --
//
// Xy
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

namespace sheetimageprocessor
{

class Xy
{
public:
	std::size_t x;
	std::size_t y;

	constexpr Xy()
		: x{ 0u }
		, y{ 0u }
	{
	}

	constexpr Xy(const std::size_t newX, const std::size_t newY)
	{
		x = newX;
		y = newY;
	}

	template <class T>
	constexpr Xy(const T& otherXy)
	{
		x = static_cast<std::size_t>(otherXy.x);
		y = static_cast<std::size_t>(otherXy.y);
	}
	template <class T, class U>
	constexpr Xy(const T newX, const U newY)
	{
		x = static_cast<std::size_t>(newX);
		y = static_cast<std::size_t>(newY);
	}

	constexpr bool operator==(const Xy other)
	{
		return ((x == other.x) && (y == other.y));
	}
	constexpr Xy& operator+=(const Xy other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}
	constexpr Xy& operator-=(const Xy other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}
	constexpr Xy& operator*=(const Xy other)
	{
		x *= other.x;
		y *= other.y;
		return *this;
	}
	constexpr Xy& operator*=(const std::size_t scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}
	constexpr Xy& operator/=(const Xy other)
	{
		x /= other.x;
		y /= other.y;
		return *this;
	}
	constexpr Xy& operator/=(const std::size_t scalar)
	{
		x /= scalar;
		y /= scalar;
		return *this;
	}

	std::string asString(const bool isSize = false, const bool includeNewline = false) const
	{
		if (isSize)
			return std::to_string(x) + "x" + std::to_string(y) + (includeNewline ? "\n" : "");
		else
			return "(" + std::to_string(x) + ", " + std::to_string(y) + ")" + (includeNewline ? "\n" : "");
	}
};

inline Xy operator+(Xy lhs, const Xy rhs)
{
	return lhs += rhs;
}
inline Xy operator-(Xy lhs, const Xy rhs)
{
	return lhs -= rhs;
}
inline Xy operator*(Xy lhs, const Xy rhs)
{
	return lhs *= rhs;
}
inline Xy operator*(Xy lhs, const std::size_t rhs)
{
	return lhs *= rhs;
}
inline Xy operator/(Xy lhs, const Xy rhs)
{
	return lhs /= rhs;
}
inline Xy operator/(Xy lhs, const std::size_t rhs)
{
	return lhs /= rhs;
}

} // namespace sheetimageprocessor

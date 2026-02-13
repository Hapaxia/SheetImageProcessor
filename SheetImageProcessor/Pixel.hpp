//////////////////////////////////////////////////////////////////////////////
//
// Sheet Image Processor (https://github.com/Hapaxia/SheetImageProcessor
// --
//
// Pixel
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

struct Pixel
{
	std::uint8_t r;
	std::uint8_t g;
	std::uint8_t b;
	std::uint8_t a;

	void setRgba(const std::uint8_t newR, const std::uint8_t newG, const std::uint8_t newB, const std::uint8_t newA)
	{
		r = newR;
		g = newG;
		b = newB;
		a = newA;
	}
	void setBgra(const std::uint8_t newB, const std::uint8_t newG, const std::uint8_t newR, const std::uint8_t newA)
	{
		b = newB;
		g = newG;
		r = newR;
		a = newA;
	}

	bool operator==(const Pixel& other) const
	{
		return ((r == other.r) && (g == other.g) && (b == other.b) && (a == other.a));
	}
};

} // namespace sheetimageprocessor

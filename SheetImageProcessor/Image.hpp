//////////////////////////////////////////////////////////////////////////////
//
// Sheet Image Processor (https://github.com/Hapaxia/SheetImageProcessor
// --
//
// Image
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
#include "Pixel.hpp"
#include "Xy.hpp"
#include "Rect.hpp"
#include "Atlas.hpp"

#include <functional>

namespace sheetimageprocessor
{

class Image
{
public:
	enum class PixelFormat // 32-bit colour: 8-bit per channel
	{
		RGBA,
		BGRA,
	};

	Image();

	void setSize(Xy size, bool clear = true, Pixel clearPixel = Pixel{ 0u, 0u, 0u, 255u });
	void setSize(Xy size, const std::uint8_t* data);
	Xy getSize() const;
	void resize(Xy newSize);
	void resize(Xy newSize, Atlas& atlas); // resizes image and atlas together, keeping them synchronised. may affect atlas tile ratios, depending on size

	void setPixel(std::size_t index, Pixel pixel);
	Pixel getPixel(std::size_t index) const;

	void setPixel(Xy location, Pixel pixel);
	Pixel getPixel(Xy location) const;

	void setPixelFormat(PixelFormat pixelFormat, bool convert = true);
	PixelFormat getPixelFormat() const;

	void setIsTopDown(bool isTopDown, bool convert = true);
	bool getIsTopDown() const;

	void flipVertically();

	void clear(Pixel pixel = Pixel{ 0u, 0u, 0u, 255u });
	void clear(Rect rect, Pixel pixel = Pixel{ 0u, 0u, 0u, 255u });
	void flip(bool horiz, bool vert, Rect rect = Rect{});
	void rotate(Rect rect = Rect{}, bool clockwise = true);
	void copy(Xy position, const Image& sourceImage, Rect sourceRect);
	Rect copy(Xy position, const Image& sourceImage, Rect sourceRect, std::size_t expansion);
	Rect expand(Rect rect, std::size_t expansion = 1u); // returns the expanded Rect. NOTE: expanded Rect MUST fit within the image otherwise an exception is thrown
	void crop(Rect rect);
	void invert(Rect rect = Rect{});
	void replacePixel(Pixel newPixel, Pixel origPixel, Rect rect = Rect{});
	void fill(Xy startPosition, Pixel replacementPixel, Rect boundary, double tolerance);
	void fill(Xy startPosition, Pixel replacementPixel, Pixel targetPixel, Rect boundary, double tolerance);
	void fill(Xy startPosition, Pixel replacementPixel, Rect boundary = Rect{}, Pixel tolerance = Pixel{});
	void fill(Xy startPosition, Pixel replacementPixel, Pixel targetPixel, Rect boundary = Rect{}, Pixel tolerance = Pixel{});

	void processPixels(const std::function<void(Pixel&)>& pixelProcessFunction, Rect rect = Rect{});
	void processPixels(const std::function<void(Pixel&, const Xy xy)>& pixelProcessFunction, Rect rect = Rect{});

	void expand(const Atlas& atlas, std::size_t expansion = 1u); // does not affect atlas - cannot expand its tiles
	void expand(Atlas& atlas, bool expandAtlasTiles = false, std::size_t expansion = 1u);
	bool transfer(Atlas& atlas, const Image& sourceImage, const Atlas& sourceAtlas, std::size_t amountOfExpansionIncluded = 0u);
	void trimAtlas(Atlas& atlas, Pixel pixelToTrim = Pixel{ 0u, 0u, 0u, 0u }) const;

	Xy joinGridTiles(
		Xy startPosition,
		Xy gridSize,
		Xy tileSize,
		Xy separation,
		bool emptyOrig = false,
		Pixel emptyPixel = Pixel{});
	Xy separateGridTiles(
		Xy startPosition,
		Xy offset,
		Xy gridSize,
		Xy tileSize,
		Xy separation,
		std::size_t expansion,
		Xy origSeparation = { 0u, 0u },
		bool emptyOrig = false,
		Pixel emptyPixel = Pixel{});
	Atlas separateGridTilesReturnAtlas(
		Xy startPosition,
		Xy offset,
		Xy gridSize,
		Xy tileSize,
		Xy separation,
		std::size_t expansion,
		Xy origSeparation = { 0u, 0u },
		bool emptyOrig = false,
		Pixel emptyPixel = Pixel{},
		std::size_t category = 0u,
		std::size_t initId = 0u);

	const std::uint8_t* getData() const;

private:
	const std::size_t m_numberOfValuesPerPixel;
	bool m_isTopDown;
	PixelFormat m_pixelFormat;
	Xy m_size;
	std::vector<std::uint8_t> m_data;

	bool priv_isValidIndex(const std::size_t index) const;
	std::size_t priv_getIndexFromLocation(const Xy location) const;
	void priv_setPixel(const std::size_t index, const Pixel& pixel);
	Pixel priv_getPixel(const std::size_t index) const;
	bool priv_rectHasNoSize(const Rect rect) const;
	void priv_makeRectFullImageSizeIfHasNoSize(Rect& rect) const;
};

} // namespace sheetimageprocessor
#include "Image.inl"

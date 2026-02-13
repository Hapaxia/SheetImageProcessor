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
#include "Image.hpp"
#include "Atlas.hpp"

#include <queue>

//#include <iostream>

namespace sheetimageprocessor
{

inline Image::Image()
	: m_numberOfValuesPerPixel{ 4u }
	, m_isTopDown{ true }
	, m_pixelFormat{ PixelFormat::RGBA }
	, m_size{ 0u, 0u }
	, m_data{}
{

}

inline void Image::setSize(const Xy size, const bool clear, const Pixel clearPixel)
{
	if ((size == m_size) && (!clear))
		return;
	if ((m_size.x * m_size.y) != (size.x * size.y))
		m_data.resize(size.x * size.y * m_numberOfValuesPerPixel);
	m_size = size;
	if (clear)
	{
		const std::size_t numberOfPixels{ m_size.x * m_size.y };
		for (std::size_t i{ 0u }; i < numberOfPixels; ++i)
			setPixel(i, clearPixel);
	}
}

inline void Image::setSize(const Xy size, const std::uint8_t* data)
{
	setSize(size, false);
	const std::size_t dataSize{ m_data.size()};
	for (std::size_t i{ 0u }; i < dataSize; ++i)
		m_data[i] = data[i];
}

inline Xy Image::getSize() const
{
	return m_size;
}

inline void Image::resize(const Xy newSize)
{
	Image result{};
	result.setIsTopDown(getIsTopDown());
	result.setPixelFormat(getPixelFormat());
	result.setSize(newSize);
	for (std::size_t y{ 0u }; y < newSize.y; ++y)
	{
		const std::size_t sourceY{ y * m_size.y / newSize.y };
		for (std::size_t x{ 0u }; x < newSize.x; ++x)
		{
			const std::size_t sourceX{ x * m_size.x / newSize.x };
			result.setPixel({ x, y }, getPixel({ sourceX, sourceY }));
		}
	}
	m_size = newSize;
	m_data = result.m_data;
}

inline void Image::resize(const Xy newSize, Atlas& atlas)
{
	const Xy origSize{ m_size };
	resize(newSize);

	const float scaleX{ static_cast<float>(newSize.x) / origSize.x };
	const float scaleY{ static_cast<float>(newSize.y) / origSize.y };
	atlas.scale(scaleX, scaleY);
	return;
}

inline void Image::setPixel(const std::size_t index, const Pixel pixel)
{
	if (!priv_isValidIndex(index))
		return;
	priv_setPixel(index, pixel);
}

inline Pixel Image::getPixel(const std::size_t index) const
{
	if (!priv_isValidIndex(index))
		return {};
	return priv_getPixel(index);
}

inline void Image::setPixel(const Xy location, Pixel pixel)
{
	const std::size_t index{ priv_getIndexFromLocation(location) };
	setPixel(index, pixel);
}

inline Pixel Image::getPixel(const Xy location) const
{
	const std::size_t index{ priv_getIndexFromLocation(location) };
	return getPixel(index);
}

inline void Image::setPixelFormat(const PixelFormat pixelFormat, const bool convert)
{
	if (pixelFormat == m_pixelFormat)
		return;
	m_pixelFormat = pixelFormat;

	if (convert)
	{
		const std::size_t dataSize{ m_data.size() };
		for (std::size_t i{ 0u }; i < dataSize; i += 4u)
		{
			const std::uint8_t tempValue{ m_data[i] };
			m_data[i] = m_data[i + 2u];
			m_data[i + 2u] = tempValue;
		}
	}
}

inline Image::PixelFormat Image::getPixelFormat() const
{
	return m_pixelFormat;
}

inline void Image::setIsTopDown(bool isTopDown, const bool convert)
{
	if (isTopDown == m_isTopDown)
		return;
	m_isTopDown = isTopDown;
	if (convert)
		flipVertically();
}

inline bool Image::getIsTopDown() const
{
	return m_isTopDown;
}

inline void Image::flipVertically()
{
	const std::size_t halfHeight{ m_size.y / 2u };
	for (std::size_t y{ 0u }; y < halfHeight; ++y)
	{
		const std::size_t oppositeY{ m_size.y - y - 1u };
		for (std::size_t x{ 0u }; x < m_size.x; ++x)
		{
			Pixel temp{ getPixel({ x, y }) };
			setPixel({ x, y }, getPixel({ x, oppositeY }));
			setPixel({ x, oppositeY }, temp);
		}
	}
}

inline void Image::clear(const Pixel pixel)
{
	const std::size_t numberOfPixels{ m_data.size() };
	for (std::size_t i{ 0u }; i < numberOfPixels; ++i)
		setPixel(i, pixel);
}

inline void Image::clear(const Rect rect, const Pixel pixel)
{
	if ((rect.position.x >= m_size.x) ||
		(rect.position.y >= m_size.y) ||
		((rect.position.x + rect.size.x) > m_size.x) ||
		((rect.position.y + rect.size.y) > m_size.y))
		return;
	for (std::size_t y{ 0u }; y < rect.size.y; ++y)
	{
		const std::size_t rectY{ rect.position.y + y };
		for (std::size_t x{ 0u }; x < rect.size.x; ++x)
			setPixel({ rect.position.x + x, rectY }, pixel);
	}
}

inline void Image::flip(const bool horiz, const bool vert, Rect rect)
{
	if (!(horiz || vert))
		return;
	if ((rect.position.x >= m_size.x) ||
		(rect.position.y >= m_size.y) ||
		((rect.position.x + rect.size.x) > m_size.x) ||
		((rect.position.y + rect.size.y) > m_size.y))
		return;

	priv_makeRectFullImageSizeIfHasNoSize(rect);
	const std::size_t xLimit{ (horiz && !vert) ? rect.size.x / 2u : rect.size.x };
	const std::size_t yLimit{ vert ? rect.size.y / 2u : rect.size.y };
	for (std::size_t y{ 0u }; y < yLimit; ++y)
	{
		const std::size_t oppositeY{ vert ? (rect.size.y - y - 1u) : y };
		for (std::size_t x{ 0u }; x < xLimit; ++x)
		{
			const std::size_t oppositeX{ horiz ? (rect.size.x - x - 1u) : x };
			Pixel temp{ getPixel({ rect.position.x + x, rect.position.y + y }) };
			setPixel({ rect.position.x + x, rect.position.y + y }, getPixel({ rect.position.x + oppositeX, rect.position.y + oppositeY }));
			setPixel({ rect.position.x + oppositeX, rect.position.y + oppositeY }, temp);
		}
	}
}

inline void Image::rotate(Rect rect, const bool clockwise)
{
	if (rect.size.x != rect.size.y)
		return;
	if ((rect.position.x >= m_size.x) ||
		(rect.position.y >= m_size.y) ||
		((rect.position.x + rect.size.x) > m_size.x) ||
		((rect.position.y + rect.size.y) > m_size.y))
		return;

	priv_makeRectFullImageSizeIfHasNoSize(rect);
	const std::size_t size{ rect.size.x };
	const std::size_t halfSize{ size / 2u };
	for (std::size_t layer{ 0u }; layer < halfSize; ++layer)
	{
		std::size_t oppositeLayer{ size - layer - 1u };
		for (std::size_t a{ layer }; (a + 1u) < (size - layer); ++a)
		{
			std::size_t oppositeA{ size - a - 1u };
			if (clockwise)
			{
				const Pixel temp{ getPixel({ rect.position.x + a, rect.position.y + layer }) };
				setPixel({ rect.position.x + a, rect.position.y + layer }, getPixel({ rect.position.x + layer, rect.position.y + oppositeA }));
				setPixel({ rect.position.x + layer, rect.position.y + oppositeA }, getPixel({ rect.position.x + oppositeA, rect.position.y + oppositeLayer }));
				setPixel({ rect.position.x + oppositeA, rect.position.y + oppositeLayer }, getPixel({ rect.position.x + oppositeLayer, rect.position.y + a }));
				setPixel({ rect.position.x + oppositeLayer, rect.position.y + a }, temp);
			}
			else
			{
				const Pixel temp{ getPixel({ rect.position.x + layer, rect.position.y + a }) };
				setPixel({ rect.position.x + layer, rect.position.y + a }, getPixel({ rect.position.x + oppositeA, rect.position.y + layer }));
				setPixel({ rect.position.x + oppositeA, rect.position.y + layer }, getPixel({ rect.position.x + oppositeLayer, rect.position.y + oppositeA }));
				setPixel({ rect.position.x + oppositeLayer, rect.position.y + oppositeA }, getPixel({ rect.position.x + a, rect.position.y + oppositeLayer }));
				setPixel({ rect.position.x + a, rect.position.y + oppositeLayer }, temp);

			}
		}
	}
}

inline void Image::copy(Xy position, const Image& sourceImage, Rect sourceRect)
{
	const Xy sourceImageSize{ sourceImage.getSize() };
	if ((sourceRect.size.x == 0u) || (sourceRect.size.y == 0u))
	{
		sourceRect.size.x = sourceImageSize.x - position.x;
		sourceRect.size.y = sourceImageSize.y - position.y;
	}
	if ((sourceRect.position.x >= sourceImageSize.x) ||
		(sourceRect.position.y >= sourceImageSize.y) ||
		((sourceRect.position.x + sourceRect.size.x) > sourceImageSize.x) ||
		((sourceRect.position.y + sourceRect.size.y) > sourceImageSize.y))
		return;

	for (std::size_t y{ 0u }; y < sourceRect.size.y; ++y)
	{
		const std::size_t sourceY{ sourceRect.position.y + y };
		const std::size_t destinationY{ position.y + y };
		for (std::size_t x{ 0u }; x < sourceRect.size.x; ++x)
		{
			const std::size_t sourceX{ sourceRect.position.x + x };
			const std::size_t destinationX{ position.x + x };
			setPixel({ destinationX, destinationY }, sourceImage.getPixel({ sourceX, sourceY }));
		}
	}
}

inline Rect Image::copy(Xy position, const Image& sourceImage, Rect sourceRect, const std::size_t expansion)
{
	const Xy sourceImageSize{ sourceImage.getSize() };
	if ((sourceRect.size.x == 0u) || (sourceRect.size.y == 0u))
	{
		sourceRect.size.x = sourceImageSize.x - position.x;
		sourceRect.size.y = sourceImageSize.y - position.y;
	}
	if ((sourceRect.position.x >= sourceImageSize.x) ||
		(sourceRect.position.y >= sourceImageSize.y) ||
		(position.x < expansion) ||
		(position.y < expansion) ||
		((sourceRect.position.x + sourceRect.size.x) > sourceImageSize.x) ||
		((sourceRect.position.y + sourceRect.size.y) > sourceImageSize.y))
		return {};

	position -= { expansion, expansion };
	const Xy expandedSize{ sourceRect.size + Xy{ expansion + expansion, expansion + expansion } };
	for (std::size_t y{ 0u }; y < expandedSize.y; ++y)
	{
		std::size_t sourceY{ sourceRect.position.y };
		if (y >= expansion)
		{
			if (y < (sourceRect.size.y + expansion))
				sourceY = sourceRect.position.y + y - expansion;
			else
				sourceY = sourceRect.position.y + sourceRect.size.y - 1u;
		}
		const std::size_t destinationY{ position.y + y };
		for (std::size_t x{ 0u }; x < expandedSize.x; ++x)
		{
			std::size_t sourceX{ sourceRect.position.x };
			if (x >= expansion)
			{
				if (x < (sourceRect.size.x + expansion))
					sourceX = sourceRect.position.x + x - expansion;
				else
					sourceX = sourceRect.position.x + sourceRect.size.x - 1u;
			}
			const std::size_t destinationX{ position.x + x };
			setPixel({ destinationX, destinationY }, sourceImage.getPixel({ sourceX, sourceY }));
		}
	}
	return { position, expandedSize };
}

inline Rect Image::expand(const Rect rect, const std::size_t expansion)
{
	bool doesNotFit{ false };
	doesNotFit = (rect.position.x < expansion) || (rect.position.y < expansion);
	if (!doesNotFit)
	{
		const Xy posAndSize{ rect.position + rect.size };
		if (((posAndSize.x + expansion) > m_size.x) || ((posAndSize.y + expansion) > m_size.y))
			doesNotFit = true;
	}
	if (doesNotFit)
		throw(Exception("expanded rect does not fit inside image."));

	const std::size_t doubleExpansion{ expansion * 2u };
	Rect expandedRect{ rect };
	expandedRect.position -= { expansion, expansion };
	expandedRect.size += { doubleExpansion, doubleExpansion };
	const std::size_t longestDimension{ std::max(rect.size.x, rect.size.y) };
	const std::size_t shortestDimension{ std::min(rect.size.x, rect.size.y) };
	const bool isWider{ rect.size.x > rect.size.y };
	for (std::size_t e{ 1u }; e <= expansion; ++e)
	{
		const Xy leftTop{ rect.position - Xy{ e, e } };
		const Xy rightBottom{ rect.position + rect.size + Xy{ e - 1u, e - 1u } };
		const std::size_t doubleE{ e * 2u };
		for (std::size_t i{ 1u }; i < longestDimension + doubleE; ++i)
		{
			const bool isBothDimensions{ i < (shortestDimension + doubleE) };
			if (isBothDimensions || isWider)
			{
				Xy top{ leftTop.x + i, leftTop.y }; // top-left going right (missing top-left corner)
				Xy bottom{ rightBottom.x - i, rightBottom.y }; // bottom-right going left (missing bottom-right corner)
				setPixel(top, getPixel(top + Xy{ 0u, 1u }));
				setPixel(bottom, getPixel(bottom - Xy{ 0u, 1u }));
			}
			if (isBothDimensions || !isWider)
			{
				Xy left{ leftTop.x, rightBottom.y - i }; // bottom-left going up (missing bottom-left corner)
				Xy right{ rightBottom.x, leftTop.y + i }; // top-right going down (missing top-right corner)
				setPixel(left, getPixel(left + Xy{ 1u, 0u }));
				setPixel(right, getPixel(right - Xy{ 1u, 0u }));
			}
		}
	}

	return expandedRect;
}

inline void Image::crop(Rect rect)
{
	if ((rect.size.x == 0u) || (rect.size.y == 0u))
	{
		setSize({ 0u, 0u });
		return;
	}
	Image copyImage{};
	copyImage.setSize(rect.size);
	copyImage.copy({ 0u, 0u }, *this, rect);
	setSize(rect.size);
	rect.position = { 0u, 0u };
	copy({ 0u, 0u }, copyImage, rect);
}

inline void Image::invert(Rect rect)
{
	priv_makeRectFullImageSizeIfHasNoSize(rect);
	for (std::size_t y{ 0u }; y < rect.size.y; ++y)
	{
		for (std::size_t x{ 0u }; x < rect.size.x; ++x)
		{
			const Xy xy{ rect.position.x + x, rect.position.y + y };
			Pixel pixel{ getPixel(xy) };
			pixel.r = 255u - pixel.r;
			pixel.g = 255u - pixel.g;
			pixel.b = 255u - pixel.b;
			setPixel(xy, pixel);
		}
	}
}

inline void Image::replacePixel(const Pixel newPixel, const Pixel origPixel, Rect rect)
{
	priv_makeRectFullImageSizeIfHasNoSize(rect);
	for (std::size_t y{ 0u }; y < rect.size.y; ++y)
	{
		for (std::size_t x{ 0u }; x < rect.size.x; ++x)
		{
			const Xy xy{ rect.position.x + x, rect.position.y + y };
			if (getPixel(xy) == origPixel)
				setPixel(xy, newPixel);
		}
	}
}

inline void Image::fill(const Xy startPosition, const Pixel pixel, Rect boundary, const double tolerance)
{
	priv_makeRectFullImageSizeIfHasNoSize(boundary);
	if (!boundary.contains(startPosition))
		return;

	const Pixel startPixel{ getPixel(startPosition) };
	fill(startPosition, pixel, startPixel, boundary, tolerance);
}

inline void Image::fill(Xy startPosition, Pixel replacementPixel, Pixel targetPixel, Rect boundary, double tolerance)
{
	priv_makeRectFullImageSizeIfHasNoSize(boundary);
	if (!boundary.contains(startPosition))
		return;

	if (tolerance <= 0.0)
	{
		fill(startPosition, replacementPixel, targetPixel, boundary, Pixel{ 0u, 0u, 0u, 0u });
		return;
	}

	if (tolerance > 1.0)
		tolerance = 1.0;
	const Pixel startPixel{ getPixel(startPosition) };
	const Xy position{ startPosition };
	Image& image{ *this };
	auto amountOfDifferenceBetweenTwoPixels = [](const Pixel& pixel, const Pixel& reference)
	{
		const int differenceR{ static_cast<int>(pixel.r) - static_cast<int>(reference.r) };
		const int differenceG{ static_cast<int>(pixel.g) - static_cast<int>(reference.g) };
		const int differenceB{ static_cast<int>(pixel.b) - static_cast<int>(reference.b) };
		const int differenceA{ static_cast<int>(pixel.a) - static_cast<int>(reference.a) };
		constexpr double maxDifference{ 255.0 + 255.0 + 255.0 + 255.0 };
		constexpr double maxDifferenceMultiplier{ 1.0 / maxDifference };
		return (static_cast<double>(std::abs(differenceR)) + static_cast<double>(std::abs(differenceG)) +
			static_cast<double>(std::abs(differenceB)) + static_cast<double>(std::abs(differenceA))) * maxDifferenceMultiplier;
	};
	std::queue<Xy> q{};
	q.push(position);
	std::size_t largestQueueSize{ 0u };
	while (!q.empty())
	{
		const Xy xy{ q.front() };
		q.pop();
		if (boundary.contains(xy))
		{
			const Pixel currentPixel{ image.getPixel(xy) };
			if (currentPixel != replacementPixel)
			{
				if (amountOfDifferenceBetweenTwoPixels(currentPixel, targetPixel) <= tolerance)
				{
					image.setPixel(xy, replacementPixel);
					if (xy.x > boundary.position.x)
						q.push({ xy.x - 1u, xy.y });
					if ((xy.x + 1u) < boundary.position.x + boundary.size.x)
						q.push({ xy.x + 1u, xy.y });
					if (xy.y > boundary.position.y)
						q.push({ xy.x, xy.y - 1u });
					if ((xy.y + 1u) < boundary.position.y + boundary.size.y)
						q.push({ xy.x, xy.y + 1u });
				}
			}
		}
		largestQueueSize = std::max(largestQueueSize, q.size());
	}
}

inline void Image::fill(Xy startPosition, Pixel pixel, Rect boundary, Pixel tolerance)
{
	priv_makeRectFullImageSizeIfHasNoSize(boundary);
	if (!boundary.contains(startPosition))
		return;

	const Pixel startPixel{ getPixel(startPosition) };
	fill(startPosition, pixel, startPixel, boundary, tolerance);
}

inline void Image::fill(Xy startPosition, Pixel replacementPixel, Pixel targetPixel, Rect boundary, Pixel tolerance)
{
	priv_makeRectFullImageSizeIfHasNoSize(boundary);
	if (!boundary.contains(startPosition))
		return;

	const Xy position{ startPosition };
	Image& image{ *this };
	auto differenceBetweenTwoPixels = [](const Pixel& pixel, const Pixel& reference)
	{
		const int differenceR{ static_cast<int>(pixel.r) - static_cast<int>(reference.r) };
		const int differenceG{ static_cast<int>(pixel.g) - static_cast<int>(reference.g) };
		const int differenceB{ static_cast<int>(pixel.b) - static_cast<int>(reference.b) };
		const int differenceA{ static_cast<int>(pixel.a) - static_cast<int>(reference.a) };
		return Pixel{ static_cast<std::uint8_t>(std::abs(differenceR)), static_cast<std::uint8_t>(std::abs(differenceG)),
			static_cast<std::uint8_t>(std::abs(differenceB)), static_cast<std::uint8_t>(std::abs(differenceA)) };
	};
	const bool isZeroTolerance{ (tolerance.r == 0u) && (tolerance.g == 0u) && (tolerance.b == 0u) && (tolerance.a == 0u) };
	std::queue<Xy> q{};
	q.push(position);
	while (!q.empty())
	{
		const Xy xy{ q.front() };
		q.pop();
		if (boundary.contains(xy))
		{
			const Pixel currentPixel{ image.getPixel(xy) };
			if (currentPixel != replacementPixel)
			{
				bool isToBeReplaced{ isZeroTolerance && (currentPixel == targetPixel) };
				if (!isToBeReplaced)
				{
					const Pixel difference{ differenceBetweenTwoPixels(currentPixel, targetPixel) };
					isToBeReplaced = (difference.r <= tolerance.r) && (difference.g <= tolerance.g) && (difference.b <= tolerance.b) && (difference.a <= tolerance.a);
				}
				if (isToBeReplaced)
				{
					image.setPixel(xy, replacementPixel);
					if (xy.x > boundary.position.x)
						q.push({ xy.x - 1u, xy.y });
					if ((xy.x + 1u) < boundary.position.x + boundary.size.x)
						q.push({ xy.x + 1u, xy.y });
					if (xy.y > boundary.position.y)
						q.push({ xy.x, xy.y - 1u });
					if ((xy.y + 1u) < boundary.position.y + boundary.size.y)
						q.push({ xy.x, xy.y + 1u });
				}
			}
		}
	}
}

inline void Image::processPixels(const std::function<void(Pixel&)>& pixelProcessFunction, Rect rect)
{
	priv_makeRectFullImageSizeIfHasNoSize(rect);
	for (std::size_t y{ 0u }; y < rect.size.y; ++y)
	{
		for (std::size_t x{ 0u }; x < rect.size.x; ++x)
		{
			const Xy xy{ rect.position.x + x, rect.position.y + y };
			Pixel pixel{ getPixel(xy) };
			pixelProcessFunction(pixel);
			setPixel(xy, pixel);
		}
	}
}

inline void Image::processPixels(const std::function<void(Pixel&, const Xy)>& pixelProcessFunction, Rect rect)
{
	priv_makeRectFullImageSizeIfHasNoSize(rect);
	for (std::size_t y{ 0u }; y < rect.size.y; ++y)
	{
		for (std::size_t x{ 0u }; x < rect.size.x; ++x)
		{
			const Xy localXy{ x, y };
			const Xy xy{ rect.position + localXy };
			Pixel pixel{ getPixel(xy) };
			pixelProcessFunction(pixel, localXy);
			setPixel(xy, pixel);
		}
	}
}

inline void Image::expand(const Atlas& atlas, const std::size_t expansion)
{
	const std::size_t atlasSize{ atlas.getSize() };
	for (std::size_t i{ 0u }; i < atlasSize; ++i)
		expand(atlas.get(i).rect, expansion);
}

inline void Image::expand(Atlas& atlas, const bool expandAtlasTiles, const std::size_t expansion)
{
	const std::size_t atlasSize{ atlas.getSize() };
	for (std::size_t i{ 0u }; i < atlasSize; ++i)
	{
		const Rect expandedRect{ expand(atlas.get(i).rect, expansion) };
		if (expandAtlasTiles)
			atlas.access(i).rect = expandedRect;
	}
}

inline bool Image::transfer(Atlas& atlas, const Image& sourceImage, const Atlas& sourceAtlas, const std::size_t amountOfExpansionIncluded)
{
	const std::size_t atlasSize{ atlas.getSize() };
	const std::size_t sourceAtlasSize{ sourceAtlas.getSize() };
	if (atlasSize != sourceAtlasSize)
		return false;

	for (std::size_t i{ 0u }; i < atlasSize; ++i)
		copy(atlas.get(i).rect.position, sourceImage, sourceAtlas.get(i).rect, amountOfExpansionIncluded);
	return true;
}

inline void Image::trimAtlas(Atlas& atlas, const Pixel pixelToTrim) const
{
	const std::size_t numberOfTiles{ atlas.getSize() };
	for (std::size_t tileIndex{ 0u }; tileIndex < numberOfTiles; ++tileIndex)
	{
		Atlas::Tile tile{ atlas.get(tileIndex) };

		if ((tile.rect.size.x == 0u) || (tile.rect.size.y == 0u))
		{
			tile.rect.size = { 0u, 0u };
			tile.offset = { 0u, 0u };
			continue;
		}

		const Xy origOffset{ tile.offset };

		const Xy initPosition{ tile.rect.position };

		tile.offset.y = tile.rect.size.y; // used to track if any content was detected during 'trim top': entire rectangle is pixelToTrim

		// trim top
		for (std::size_t y{ 0u }; y < tile.rect.size.y; ++y)
		{
			for (std::size_t x{ 0u }; x < tile.rect.size.x; ++x)
			{
				if (getPixel({ tile.rect.position.x + x, tile.rect.position.y + y }) != pixelToTrim)
				{
					tile.offset.y = y;
					tile.rect.position.y += y;
					tile.rect.size.y -= y;
					goto endloopTrimTop;
				}
			}
		}
	endloopTrimTop:

		// if no content, set rect to 0x0 (at initial position) and move on to next tile
		if (tile.offset.y == tile.rect.size.y)
		{
			tile.rect.position = initPosition;
			tile.rect.size = { 0u, 0u };
			tile.offset = { 0u, 0u };
			continue;
		}

		// trim bottom
		for (std::size_t y{ 0u }; y < tile.rect.size.y; ++y)
		{
			for (std::size_t x{ 0u }; x < tile.rect.size.x; ++x)
			{
				if (getPixel({ tile.rect.position.x + x, tile.rect.position.y + tile.rect.size.y - y - 1u }) != pixelToTrim)
				{
					tile.rect.size.y -= y;
					goto endloopTrimBottom;
				}
			}
		}
	endloopTrimBottom:

		// trim left
		for (std::size_t x{ 0u }; x < tile.rect.size.x; ++x)
		{
			for (std::size_t y{ 0u }; y < tile.rect.size.y; ++y)
			{
				if (getPixel({ tile.rect.position.x + x, tile.rect.position.y + y }) != pixelToTrim)
				{
					tile.offset.x = x;
					tile.rect.position.x += x;
					tile.rect.size.x -= x;
					goto endloopTrimLeft;
				}
			}
		}
	endloopTrimLeft:

		// trim right
		for (std::size_t x{ 0u }; x < tile.rect.size.x; ++x)
		{
			for (std::size_t y{ 0u }; y < tile.rect.size.y; ++y)
			{
				if (getPixel({ tile.rect.position.x + tile.rect.size.x - x - 1u, tile.rect.position.y + y }) != pixelToTrim)
				{
					tile.rect.size.x -= x;
					goto endloopTrimRight;
				}
			}
		}
	endloopTrimRight:

		tile.offset += origOffset; // don't ignore offset that the tiles (with content) already had 
		atlas.set(tileIndex, tile);
	}
}



inline Xy Image::joinGridTiles(
	const Xy startPosition,
	const Xy gridSize,
	const Xy tileSize,
	const Xy origSeparation,
	const bool emptyOrig,
	const Pixel emptyPixel)
{
	const Xy finalSize{ gridSize.x * tileSize.x, gridSize.y * tileSize.y };
	for (std::size_t y{ 0u }; y < finalSize.y; ++y)
	{
		for (std::size_t x{ 0u }; x < finalSize.x; ++x)
		{
			const Xy currentTile{ x / tileSize.x, y / tileSize.y };
			const Xy uv{ x % tileSize.x, y % tileSize.y };
			const Xy source{ startPosition.x + currentTile.x * (tileSize.x + origSeparation.x) + uv.x, startPosition.y + currentTile.y * (tileSize.y + origSeparation.y) + uv.y };
			setPixel({ startPosition.x + x, startPosition.y + y }, getPixel(source));
		}
	}
	if (emptyOrig)
	{
		const Xy origSize{ gridSize.x * (tileSize.x + origSeparation.x), gridSize.y * (tileSize.y + origSeparation.y) };
		for (std::size_t y{ 0u }; y < origSize.y; ++y)
		{
			for (std::size_t x{ 0u }; x < origSize.x; ++x)
			{
				if ((x < finalSize.x) && (y < finalSize.y))
					continue;
				setPixel({ startPosition.x + x, startPosition.y + y }, emptyPixel);
			}
		}
	}
	return { startPosition.x + finalSize.x, startPosition.y + finalSize.y };
}

inline Xy Image::separateGridTiles(
	const Xy startPosition,
	const Xy offset,
	const Xy gridSize,
	const Xy tileSize,
	Xy separation,
	std::size_t expansion,
	const Xy origSeparation,
	const bool emptyOrig,
	const Pixel emptyPixel)
{
	if ((expansion * 2u) > separation.x)
		expansion = separation.x / 2u;
	if ((expansion * 2u) > separation.y)
		expansion = separation.y / 2u;
	if (separation.x < origSeparation.x)
		separation.x = origSeparation.x;
	if (separation.y < origSeparation.y)
		separation.y = origSeparation.y;

	const Xy origGridSize{ gridSize.x * (tileSize.x + origSeparation.x) - origSeparation.x, gridSize.y * (tileSize.y + origSeparation.y) - origSeparation.y };
	const Xy gridSizeRequired{ gridSize.x * (tileSize.x + separation.x) - separation.x + (expansion * 2u), gridSize.y * (tileSize.y + separation.y) - separation.y + (expansion * 2u) };

	if ((startPosition.x + offset.x + gridSizeRequired.x) > m_size.x)
		return {};
	if ((startPosition.y + offset.y + gridSizeRequired.y) > m_size.y)
		return {};

	struct CopyRect
	{
		Rect source;
		Rect expanded;
	};
	std::vector<CopyRect> tileRects(gridSize.x * gridSize.y);
	for (std::size_t y{ 0u }; y < gridSize.y; ++y)
	{
		for (std::size_t x{ 0u }; x < gridSize.x; ++x)
		{
			Rect& sourceRect{ tileRects[y * gridSize.x + x].source };
			Rect& expandedRect{ tileRects[y * gridSize.x + x].expanded };

			sourceRect.size = tileSize;
			expandedRect.size = { tileSize.x + (expansion * 2u), tileSize.y + (expansion * 2u) };
			sourceRect.position = { startPosition.x + (x * (tileSize.x + origSeparation.x)), startPosition.y + (y * (tileSize.y + origSeparation.y)) };
			expandedRect.position = { startPosition.x + offset.x + (x * (tileSize.x + separation.x)), startPosition.y + offset.y + (y * (tileSize.y + separation.y)) };
		}
	}
	for (std::size_t yCounter{ 0u }; yCounter < gridSizeRequired.y; ++yCounter)
	{
		const std::size_t y{ startPosition.y + offset.y + gridSizeRequired.y - yCounter - 1u };
		for (std::size_t xCounter{ 0u }; xCounter < gridSizeRequired.x; ++xCounter)
		{
			const std::size_t x{ startPosition.x + offset.x + gridSizeRequired.x - xCounter - 1u };

			bool isClearPixel{ true };

			for (const auto& rects : tileRects)
			{
				if (rects.expanded.contains({ x, y }))
				{
					isClearPixel = false;

					Xy local{ x - rects.expanded.position.x, y - rects.expanded.position.y };
					Xy uv{};
					if (local.x < expansion)
						uv.x = 0u;
					else if (local.x >= (tileSize.x + expansion))
						uv.x = tileSize.x - 1u;
					else
						uv.x = local.x - expansion;
					if (local.y < expansion)
						uv.y = 0u;
					else if (local.y >= (tileSize.y + expansion))
						uv.y = tileSize.y - 1u;
					else
						uv.y = local.y - expansion;

					uv.x += rects.source.position.x;
					uv.y += rects.source.position.y;

					setPixel({ x, y }, getPixel(uv));
					break;
				}
			}
			if (isClearPixel)
				setPixel({ x, y }, emptyPixel);
		}
	}
	if (emptyOrig)
	{
		for (std::size_t y{ 0u }; y < origGridSize.y; ++y)
		{
			for (std::size_t x{ 0u }; x < origGridSize.x; ++x)
			{
				if ((x >= offset.x) && (y >= offset.y))
					continue;
				setPixel({ x, y }, emptyPixel);
			}
		}
	}
	return { startPosition.x + offset.x + gridSizeRequired.x, startPosition.y + offset.y + gridSizeRequired.y };
}

inline Atlas Image::separateGridTilesReturnAtlas(
	const Xy startPosition,
	const Xy offset,
	const Xy gridSize,
	const Xy tileSize,
	Xy separation,
	std::size_t expansion,
	const Xy origSeparation,
	const bool emptyOrig,
	const Pixel emptyPixel,
	const std::size_t category,
	const std::size_t initId)
{
	if ((expansion * 2u) > separation.x)
		expansion = separation.x / 2u;
	if ((expansion * 2u) > separation.y)
		expansion = separation.y / 2u;
	if (separation.x < origSeparation.x)
		separation.x = origSeparation.x;
	if (separation.y < origSeparation.y)
		separation.y = origSeparation.y;

	const Xy origGridSize{ gridSize.x * (tileSize.x + origSeparation.x) - origSeparation.x, gridSize.y * (tileSize.y + origSeparation.y) - origSeparation.y };
	const Xy gridSizeRequired{ gridSize.x * (tileSize.x + separation.x) - separation.x + (expansion * 2u), gridSize.y * (tileSize.y + separation.y) - separation.y + (expansion * 2u) };

	if ((startPosition.x + offset.x + gridSizeRequired.x) > m_size.x)
		return {};
	if ((startPosition.y + offset.y + gridSizeRequired.y) > m_size.y)
		return {};

	struct CopyRect
	{
		Rect source;
		Rect expanded;
	};
	std::vector<CopyRect> tileRects(gridSize.x * gridSize.y);
	for (std::size_t y{ 0u }; y < gridSize.y; ++y)
	{
		for (std::size_t x{ 0u }; x < gridSize.x; ++x)
		{
			Rect& sourceRect{ tileRects[y * gridSize.x + x].source };
			Rect& expandedRect{ tileRects[y * gridSize.x + x].expanded };

			sourceRect.size = tileSize;
			expandedRect.size = { tileSize.x + (expansion * 2u), tileSize.y + (expansion * 2u) };
			sourceRect.position = { startPosition.x + (x * (tileSize.x + origSeparation.x)), startPosition.y + (y * (tileSize.y + origSeparation.y)) };
			expandedRect.position = { startPosition.x + offset.x + (x * (tileSize.x + separation.x)), startPosition.y + offset.y + (y * (tileSize.y + separation.y)) };
		}
	}
	for (std::size_t yCounter{ 0u }; yCounter < gridSizeRequired.y; ++yCounter)
	{
		const std::size_t y{ startPosition.y + offset.y + gridSizeRequired.y - yCounter - 1u };
		for (std::size_t xCounter{ 0u }; xCounter < gridSizeRequired.x; ++xCounter)
		{
			const std::size_t x{ startPosition.x + offset.x + gridSizeRequired.x - xCounter - 1u };

			bool isClearPixel{ true };

			for (const auto& rects : tileRects)
			{
				if (rects.expanded.contains({ x, y }))
				{
					isClearPixel = false;

					Xy local{ x - rects.expanded.position.x, y - rects.expanded.position.y };
					Xy uv{};
					if (local.x < expansion)
						uv.x = 0u;
					else if (local.x >= (tileSize.x + expansion))
						uv.x = tileSize.x - 1u;
					else
						uv.x = local.x - expansion;
					if (local.y < expansion)
						uv.y = 0u;
					else if (local.y >= (tileSize.y + expansion))
						uv.y = tileSize.y - 1u;
					else
						uv.y = local.y - expansion;

					uv.x += rects.source.position.x;
					uv.y += rects.source.position.y;

					setPixel({ x, y }, getPixel(uv));
					break;
				}
			}
			if (isClearPixel)
				setPixel({ x, y }, emptyPixel);
		}
	}
	if (emptyOrig)
	{
		for (std::size_t y{ 0u }; y < origGridSize.y; ++y)
		{
			for (std::size_t x{ 0u }; x < origGridSize.x; ++x)
			{
				if ((x >= offset.x) && (y >= offset.y))
					continue;
				setPixel({ x, y }, emptyPixel);
			}
		}
	}
	Atlas atlas{};
	const std::size_t numberOfTileRects{ tileRects.size() };
	atlas.resize(numberOfTileRects);
	Atlas::Tile tile{};
	tile.category = category;
	for (std::size_t i{ 0u }; i < numberOfTileRects; ++i)
	{
		tile.rect = tileRects[i].expanded;
		tile.id = initId + i;
		atlas.set(i, tile);
	}
	return atlas;
}

inline const std::uint8_t* Image::getData() const
{
	if (m_data.empty())
		throw Exception("Cannot get data for an empty image.");

	return m_data.data();
}

inline bool Image::priv_isValidIndex(const std::size_t index) const
{
	return ((index * m_numberOfValuesPerPixel) < m_data.size());
}

inline std::size_t Image::priv_getIndexFromLocation(const Xy location) const
{
	const std::size_t outOfBoundsIndex{ m_size.x * m_size.y };
	if ((location.x >= m_size.x) || (location.y >= m_size.y))
		return outOfBoundsIndex;
	return (location.y * m_size.x) + location.x;
}

inline void Image::priv_setPixel(const std::size_t index, const Pixel& pixel)
{
	assert(priv_isValidIndex(index));

	const std::size_t dataIndexStart{ index * m_numberOfValuesPerPixel };
	const bool isRgba{ m_pixelFormat == PixelFormat::RGBA };
	m_data[dataIndexStart + 0u] = isRgba ? pixel.r : pixel.b;
	m_data[dataIndexStart + 1u] = pixel.g;
	m_data[dataIndexStart + 2u] = isRgba ? pixel.b : pixel.r;
	m_data[dataIndexStart + 3u] = pixel.a;
}

inline Pixel Image::priv_getPixel(const std::size_t index) const
{
	assert(priv_isValidIndex(index));

	Pixel pixel{};
	const std::size_t dataIndexStart{ index * m_numberOfValuesPerPixel };
	const bool isRgba{ m_pixelFormat == PixelFormat::RGBA };

	pixel.r = m_data[dataIndexStart + (isRgba ? 0u : 2u)];
	pixel.g = m_data[dataIndexStart + 1u];
	pixel.b = m_data[dataIndexStart + (isRgba ? 2u : 0u)];
	pixel.a = m_data[dataIndexStart + 3u];

	return pixel;
}

inline bool Image::priv_rectHasNoSize(const Rect rect) const
{
	return (rect.size.x == 0u) || (rect.size.y == 0u);
}

inline void Image::priv_makeRectFullImageSizeIfHasNoSize(Rect& rect) const
{
	if (priv_rectHasNoSize(rect))
		rect = { { 0u, 0u }, m_size };
}

} // namespace sheetimageprocessor

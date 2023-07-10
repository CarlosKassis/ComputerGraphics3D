#include "ViewBuffer.h"
#include <algorithm>

COLORREF ViewBuffer::ReverseColor(COLORREF color)
{
	return RGB(GetBValue(color), GetGValue(color), GetRValue(color));
}

double ViewBuffer::GetFogOpacity(double z)
{
	if (m_eFogMode == FogMode::Off)
	{
		return 1.0;
	}

	if (z < m_dFogLimitFar)
	{
		return 1.0;
	}

	return max(0.0, 1.0 - (z - m_dFogLimitFar) / FogFadeoutMargin);
}

void ViewBuffer::SetPixel(int x, int y, double z, COLORREF color, GameObject *gameObject, bool force, double opacity)
{
	if (x < 0 || y < 0 || x >= m_nWidth || y >= m_nHeight || z < m_dCloseLimit || z > m_dFarLimit)
	{
		return;
	}

	int bufferLocation = y * m_nWidth + x;
	if (force || z <= m_zBuffer[bufferLocation])
	{
		m_zBuffer[bufferLocation] = z;
		m_pGameObjects[bufferLocation] = gameObject;
	}

	int layerCount = m_pColorLayers[bufferLocation].size();
	for (int layer = 0; layer < layerCount; layer++)
	{
		// Prevent two adjacent overlapping polygons from writing to same pixel twice, can result in wireframe like behavior when object is transparent
		if (m_pColorLayers[bufferLocation][layer].GameObject == gameObject && abs(z - m_pColorLayers[bufferLocation][layer].Z) < 0.01)
		{
			return;
		}
	}

	double fogOpacity = GetFogOpacity(z);
#pragma omp critical
	{
		m_pColorLayers[bufferLocation].push_back({ ReverseColor(color), opacity, gameObject, z, fogOpacity });
	}
}

bool CompareByZ(const ColorLayer& a, const ColorLayer& b)
{
	return a.Z < b.Z;
}

void ViewBuffer::DrawColorArray()
{
	const int bufferSize = m_nWidth * m_nHeight;
	const COLORREF fogColorReversed = ReverseColor(m_cFogColor);
#pragma omp parallel
	{
#pragma omp for nowait
		for (int i = 0; i < bufferSize; i++)
		{
			if (!m_pColorLayers[i].size())
			{
				continue;
			}

			auto& colorLayers = m_pColorLayers[i];

			COLORREF backgroundColor;
			if (m_eFogMode != FogMode::Off) {
				backgroundColor = m_eFogMode == FogMode::Color ? fogColorReversed : m_pColor[i];
			}

			// Sort by depth
			std::sort(colorLayers.begin(), colorLayers.end(), CompareByZ);

			// Push background/fog layer
			colorLayers.push_back({ backgroundColor, 1.0, nullptr, DBL_MAX, 1.0 });

			double opacitySum = 0.0;
			int layerCount = colorLayers.size();

			double remainder = 1.0;

			// Combine layers
			double finalRed = 0.0, finalGreen = 0.0, finalBlue = 0.0;
			for (int layer = 0; layer < layerCount; layer++)
			{
				double fogOpacity = colorLayers[layer].FogOpacity;
				COLORREF color = colorLayers[layer].Color;
				Vector3D colorVector(GetRValue(color), GetGValue(color), GetBValue(color));

				if (m_eFogMode != FogMode::Off)
				{
					Vector3D fogColorVector;
					if (m_eFogMode ==  FogMode::Color)
						fogColorVector = (GetRValue(fogColorReversed), GetGValue(fogColorReversed), GetBValue(fogColorReversed));
					else
					{
						COLORREF backgroundColor = m_pColor[i];
						fogColorVector = Vector3D(GetRValue(backgroundColor), GetGValue(backgroundColor), GetBValue(backgroundColor));
					}

					colorVector *= fogOpacity;
					colorVector += fogColorVector * (1.0 - fogOpacity);
				}

				finalRed = remainder * fogOpacity * colorVector.X + (1.0 - remainder * fogOpacity) * finalRed;
				finalGreen = remainder * fogOpacity * colorVector.Y + (1.0 - remainder * fogOpacity) * finalGreen;
				finalBlue = remainder * fogOpacity * colorVector.Z + (1.0 - remainder * fogOpacity) * finalBlue;

				remainder -= remainder * colorLayers[layer].Opacity * remainder * fogOpacity;
			}

			// Blend layer fog opacities with background, background is fully opaque
			m_pColor[i] = RGB(finalRed, finalGreen, finalBlue);
		}
	}
}

COLORREF ViewBuffer::GetPixelForPng(int x, int y)
{
	return m_pColor[y * m_nWidth + x] << 8;
}

COLORREF ViewBuffer::GetPixel(int x, int y)
{
	return ReverseColor(m_pColor[y * m_nWidth + x]);
}

void ViewBuffer::Resize(int width, int height)
{
	const int bufferSize = width * height;

	// Allocate only if not allocated or new size is bigger
	if (!m_pColor || width * height != m_nAllocatedBufferXxY)
	{
		m_nAllocatedBufferXxY = width * height;
		delete[] m_pColor;
		delete[] m_pColorPrevious;

		delete[] m_pBGRenderedImg;
		delete[] m_zBuffer;
		delete[] m_pGameObjects;

		m_pColor = new COLORREF[bufferSize];

		m_pColorPrevious = new COLORREF[bufferSize];
		m_bHasPreviousColor = false;

		m_pBGRenderedImg = new COLORREF[bufferSize];
		m_zBuffer = new double[bufferSize];
		m_pGameObjects = new GameObject*[bufferSize];

		delete[] m_pColorLayers;
		m_pColorLayers = new std::vector<ColorLayer>[bufferSize]();
	}

#pragma omp parallel
	{
#pragma omp for nowait
		for (int i = 0; i < bufferSize; i++)
		{
			m_pColorLayers[i].clear();
		}
	}

	if (m_nWidth != width || m_nHeight != height)
	{
		m_nWidth = width;
		m_nHeight = height;
		m_bgDirty = true;
	}

	m_Info.bmiHeader.biSize = sizeof(m_Info.bmiHeader);
	m_Info.bmiHeader.biWidth = width;
	m_Info.bmiHeader.biHeight = -height;
	m_Info.bmiHeader.biPlanes = 1;
	m_Info.bmiHeader.biBitCount = 32;
	m_Info.bmiHeader.biCompression = BI_RGB;
}

void ViewBuffer::ClearZBuffer()
{
	std::fill(m_zBuffer, m_zBuffer + m_nWidth * m_nHeight, DBL_MAX);
}

void ViewBuffer::CalculateBackgroundImage()
{
	if (m_imgMode == ImageMode::Repeat)
	{
		for (int y = 0; y < m_nHeight; y++)
		{
			int repeatedY = y % m_nBGHeight;

			for (int x = 0; x < m_nWidth; x+= m_nBGWidth)
			{
				memcpy(m_pBGRenderedImg + y * m_nWidth + x, m_pBGImg + repeatedY * m_nBGWidth, min(m_nBGWidth, m_nWidth - x) * sizeof(COLORREF));
			}
		}
	}
	else
	{
		// Stretch
		double imageToViewRatioX = m_nBGWidth / (double)m_nWidth;
		double imageToViewRatioY = m_nBGHeight / (double)m_nHeight;
		for (int y = 0; y < m_nHeight; y++)
		{
			for (int x = 0; x < m_nWidth; x++)
			{
				m_pBGRenderedImg[y * m_nWidth + x] = m_pBGImg[((int)(y * imageToViewRatioY) * m_nBGWidth) + ((int)(x * imageToViewRatioX))];
			}
		}
	}
}

ViewBuffer::~ViewBuffer()
{
	delete[] m_pColor;
	delete[] m_pBGRenderedImg;
	delete[] m_zBuffer;
	delete[] m_pGameObjects;
	delete[] m_pBGImg;

	delete[] m_pColorLayers;

	//delete m_pAntiAliasing; // making some problems
}

void ViewBuffer::RenderBackground()
{
	if (m_bgType == BackgroundType::SolidColor)
	{
		std::fill_n(m_pColor, m_nWidth * m_nHeight, m_bgColor);
	}
	else
	{
		if (m_bgDirty)
		{
			CalculateBackgroundImage();
			m_bgDirty = false;
		}

		memcpy(m_pColor, m_pBGRenderedImg, m_nWidth * m_nHeight * sizeof(COLORREF));
	}
}

void ViewBuffer::SetBackgroundSolid(COLORREF color)
{
	m_bgType = BackgroundType::SolidColor;
	m_bgColor = ReverseColor(color);
}

void ViewBuffer::SetBackgroundImage(PngWrapper& pngWrapper, ImageMode imageMode)
{
	m_nBGHeight = pngWrapper.GetHeight();
	m_nBGWidth = pngWrapper.GetWidth();
	m_bgType = BackgroundType::Image;
	m_imgMode = imageMode;

	delete[] m_pBGImg;
	m_pBGImg = new COLORREF[m_nBGHeight * m_nBGWidth];

	for (int y = 0; y < m_nBGHeight; y++)
	{
		for (int x = 0; x < m_nBGWidth; x++)
		{
			m_pBGImg[y * m_nBGWidth + x] = pngWrapper.GetValue(x, y) >> 8; // Apparently >> 8 converts colors correctly, go figure..
		}
	}

	m_bgDirty = true;
}
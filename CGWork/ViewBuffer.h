#pragma once

#include <omp.h>
#include <memory>
#include "stdafx.h"
#include "PngWrapper.h"
#include "Globals.h"
#include "GameObject.h"
#include "AntiAliasing.h"



struct ColorLayer
{
	COLORREF Color;
	double Opacity;
	GameObject* GameObject;
	double Z;
	double FogOpacity;
};

class ViewBuffer
{
private:
	const double FogFadeoutMargin = 2.0; // Amount of depth it takes for object to completely fade out
	double m_dFogLimitFar = 30.0;
	COLORREF m_cFogColor = RGB(100, 100, 100);
	FogMode m_eFogMode = FogMode::Transparent;


	int m_nAllocatedBufferXxY = 0;
	BITMAPINFO m_Info;
	COLORREF* m_pColor = nullptr;

	COLORREF* m_pColorPrevious = nullptr;
	bool m_bHasPreviousColor = false;

	double* m_zBuffer = nullptr;

	int m_nWidth = 0;
	int m_nHeight = 0;

	ImageMode m_imgMode = ImageMode::Repeat;
	BackgroundType m_bgType = BackgroundType::SolidColor;

	COLORREF m_bgColor = RGB(0, 0, 0);
	int m_nBGWidth;
	int m_nBGHeight;
	bool m_bgDirty = false;

	double m_dCloseLimit;
	double m_dFarLimit;

	COLORREF* m_pBGImg = nullptr;
	COLORREF* m_pBGRenderedImg = nullptr;
	GameObject** m_pGameObjects = nullptr;
	std::vector<ColorLayer>* m_pColorLayers = nullptr;

	AntiAliasing m_AntiAliasing;

private:
	static COLORREF ReverseColor(COLORREF color);
	double GetFogOpacity(double z);
	void CalculateBackgroundImage();

public:
	~ViewBuffer();

	ViewBuffer()
	{
	}

	int AASize() { return m_AntiAliasing.GetSize(); }
	int AAType() { return m_AntiAliasing.GetType(); }
	int Height() { return m_nHeight; }
	int Width() { return m_nWidth; }
	COLORREF* Memory() { return m_pColor; }
	BITMAPINFO Info() { return m_Info; }

	void Resize(int width, int height);
	void ClearZBuffer();
	void RenderBackground();
	void SetBackgroundSolid(COLORREF color);
	void SetBackgroundImage(PngWrapper &pngWrapper, ImageMode imageMode);
	void SetPixel(int x, int y, double z, COLORREF color, GameObject *gameObject, bool force = false, double opacity = 1.0);
	void DrawColorArray();
	COLORREF GetPixelForPng(int x, int y);
	COLORREF GetPixel(int x, int y);

	void SetLimits(double front, double behind)
	{
		m_dCloseLimit = front;
		m_dFarLimit = behind;
	}

	void ClearGameObjects()
	{
		memset(m_pGameObjects, 0, m_nHeight * m_nWidth * sizeof(GameObject*));
	}

	void DrawBlob(int y, int x, int width, COLORREF color)
	{
		for (int i = -width; i <= width; i++)
		{
			for (int j = -width; j <= width; j++)
			{
				int drawY = y + i;
				int drawX = x + j;
				if (drawX < 0 || drawY < 0 || drawX >= m_nWidth || drawY >= m_nHeight)
				{
					continue;
				}

				m_pColor[drawY * m_nWidth + drawX] = color;
			}
		}
	}

	GameObject* GetGameObject(int y, int x)
	{
		if (y < 0 || x < 0 || y >= m_nHeight || x >= m_nWidth)
		{
			return nullptr;
		}

		return m_pGameObjects[y * m_nWidth + x];
	}

	void SaveViewBufferStateForNextMotionBlur()
	{
		memcpy(m_pColorPrevious, m_pColor, m_nWidth * m_nHeight * sizeof(COLORREF));
		m_bHasPreviousColor = true;
	}

	void DrawSilhouette(COLORREF color)
	{
		int width = 1;
		COLORREF silhouetteColor = ReverseColor(color);
#pragma omp parallel
		{
#pragma omp for nowait
			for (int y = 0; y < m_nHeight; y++)
			{
				GameObject* currentGameObject = m_pGameObjects[y * m_nWidth];
				int bufferLineLocation = y * m_nWidth;
				for (int x = 0; x < m_nWidth; x++)
				{
					int bufferLocation = bufferLineLocation + x;
					if (m_pGameObjects[bufferLocation] != currentGameObject)
					{
						currentGameObject = m_pGameObjects[bufferLocation];
						DrawBlob(y, x, width, silhouetteColor);
					}

					GameObject* adjacentGameObject = nullptr;
					if ((adjacentGameObject = GetGameObject(y - 1, x)) != nullptr && adjacentGameObject != currentGameObject)
					{
						DrawBlob(y - 1, x, width, silhouetteColor);
					}

					if ((adjacentGameObject = GetGameObject(y + 1, x)) != nullptr && adjacentGameObject != currentGameObject)
					{
						DrawBlob(y + 1, x, width, silhouetteColor);
					}
				}
			}
		}
	}

	void DoMotionBlur(double prevFrameRatio = 0.25)
	{
		if (!m_bHasPreviousColor)
		{
			return;
		}

#pragma omp parallel
		{
#pragma omp for nowait
			for (int y = 0; y < m_nHeight; y++)
			{
				int bufferLineLocation = y * m_nWidth;
				for (int x = 0; x < m_nWidth; x++)
				{
					int bufferLocation = bufferLineLocation + x;
					int red = GetRValue(m_pColor[bufferLocation]) * (1.0 - prevFrameRatio) + prevFrameRatio * GetRValue(m_pColorPrevious[bufferLocation]);
					int green = GetGValue(m_pColor[bufferLocation]) * (1.0 - prevFrameRatio) + prevFrameRatio * GetGValue(m_pColorPrevious[bufferLocation]);
					int blue = GetBValue(m_pColor[bufferLocation]) * (1.0 - prevFrameRatio) + prevFrameRatio * GetBValue(m_pColorPrevious[bufferLocation]);
					m_pColor[bufferLocation] = RGB(red, green, blue);
				}
			}
		}
	}

	void AntiAliase()
	{
		m_AntiAliasing.AntiAliase(m_pColor, m_nHeight, m_nWidth);
	}

	void SetAAParams(int size, int type) {
		m_AntiAliasing.SetParams((KernelType)type, (KernelSize)size);
	}

	void SetFogParams(double farLimit, COLORREF fogColor, FogMode mode) {
		m_dFogLimitFar = farLimit;
		m_cFogColor = fogColor;
		m_eFogMode = mode;
	}
};


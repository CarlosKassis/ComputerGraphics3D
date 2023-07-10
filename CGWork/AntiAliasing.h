#pragma once
#include <omp.h>
#include <afxwin.h>
#include "Globals.h"




class AntiAliasing
{
	KernelSize m_eKernelSize = KernelSize::Size3;
	KernelType m_eKernelType = KernelType::None;

	const int kernel[8][5][5] =
	{
		// 3x3:
		{
			// Box Kernel
			{1, 1, 1, 0, 0},
			{1, 1, 1, 0, 0},
			{1, 1, 1, 0, 0},
			{0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0},
		},
		{
			// Triangle Kernel
			{1, 2, 1, 0, 0},
			{2, 4, 2, 0, 0},
			{1, 2, 1, 0, 0},
			{0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0}
		},
		{
			// Sinc Kernel
			{2, 3, 2, 0, 0},
			{3, 4, 3, 0, 0},
			{2, 3, 2, 0, 0},
			{0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0}
		},
		{
			// Gaussian kernel
			{1, 2, 1, 0, 0},
			{2, 5, 2, 0, 0},
			{1, 2, 1, 0, 0},
			{0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0}
		},
		// 5x5:
		{
			// Box Kernel
			{1, 1, 1, 1, 1},
			{1, 1, 1, 1, 1},
			{1, 1, 1, 1, 1},
			{1, 1, 1, 1, 1},
			{1, 1, 1, 1, 1}
		},
		{
			// Triangle Kernel
			{1, 2, 3, 2, 1},
			{2, 4, 6, 4, 2},
			{3, 6, 9, 6, 3},
			{2, 4, 6, 4, 2},
			{1, 2, 3, 2, 1}
		},
		{
			// Sinc Kernel
			{-2, -1, 0, -1, -2},
			{-1,  4, 6,  4, -1},
			{0 ,  6, 9,  6,  0},
			{-1,  4, 6,  4, -1},
			{-2, -1, 0, -1, -2}
		},
		{
			//Gaussian Kernel
			{1, 1, 1 , 1, 1},
			{1, 2, 4 , 2, 1},
			{1, 4, 10, 4, 1},
			{1, 2, 4 , 2, 1},
			{1, 1, 1 , 1, 1}
		}
	};

	int kernelNormalize[8] = { 9, 16, 24, 17, 25, 81, 33, 50 }; //Normalize
	
public:
	int GetSize() {
		return (int)m_eKernelSize;
	}

	int GetType() {
		return (int)m_eKernelType;
	}

	void SetParams(KernelType kernelType, KernelSize kernelSize)
	{
		m_eKernelType = kernelType;
		m_eKernelSize = kernelSize;
	}

	int accessPixel(int* arr, int col, int row, int width, int height)
	{
		int sum = 0;
		int kernelSizeHalf = m_eKernelSize == KernelSize::Size3 ? 1 : 2;
		int kernelId = (int)m_eKernelType + (m_eKernelSize == KernelSize::Size3 ? 0 : 4);
		for (int j = -kernelSizeHalf; j <= kernelSizeHalf; j++)
		{
			for (int i = -kernelSizeHalf; i <= kernelSizeHalf; i++)
			{
				if ((row + j) < 0 || (row + j) >= height || (col + i) < 0 || (col + i) >= width)
				{
					continue;
				}

				int color = arr[(row + j) * width + (col + i)];
				sum += color * kernel[kernelId][i + kernelSizeHalf][j + kernelSizeHalf];
			}
		}

		int finalChannelColor = sum / kernelNormalize[kernelId];
		return min(255, max(0, finalChannelColor));
	}
	void AntiAliase(COLORREF* buffer, int height, int width)
	{
		if (m_eKernelType == KernelType::None) return;
		int* redBuffer = new int[height * width];
		int* greenBuffer = new int[height * width];
		int* blueBuffer = new int[height * width];

#pragma omp parallel
		{
#pragma omp for nowait
			//Split channels 
			for (int y = 0; y < height; y++)
			{
				int yTimesWidth = y * width;
				for (int x = 0; x < width; x++)
				{
					int bufferLocation = yTimesWidth + x;
					COLORREF color = buffer[bufferLocation];
					redBuffer[bufferLocation] = GetRValue(color);
					greenBuffer[bufferLocation] = GetGValue(color);
					blueBuffer[bufferLocation] = GetBValue(color);
				}
			}
		}

#pragma omp parallel
		{
#pragma omp for nowait
			//Calculate filters
			for (int y = 0; y < height; y++)
			{
				int yTimesWidth = y * width;
				for (int x = 0; x < width; x++)
				{
					int bufferLocation = yTimesWidth + x;
					buffer[bufferLocation] = RGB(accessPixel(redBuffer,   x, y, width, height),
												 accessPixel(greenBuffer, x, y, width, height),
												 accessPixel(blueBuffer,  x, y, width, height));
				}
			}
		}
		delete[] redBuffer;
		delete[] greenBuffer;
		delete[] blueBuffer;

	}
};
#include "MatUtils.h"
#include "Globals.h"

void MatUtils::MatMul4x1(const double matrix[4][4], double column[4])
{
	double result[4] = { 0 };
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result[i] += matrix[i][j] * column[j];
		}
	}

	for (int i = 0; i < 4; i++)
	{
		column[i] = result[i];
	}
}

void MatUtils::MatMul4x4(const double matL[4][4], double matR[4][4], double matDest[4][4])
{
	double res[4][4] = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				res[i][j] += matL[i][k] * matR[k][j];
			}
		}
	}

	// Copy at the end in-case matDest is one of the L/R matrices
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			matDest[i][j] = res[i][j];
		}
	}
}
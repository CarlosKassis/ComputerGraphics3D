#pragma once
static class MatUtils
{
public:
	static void MatMul4x1(const double matrix[4][4], double column[4]);

	static void MatMul4x4(const double matL[4][4], double matR[4][4], double matDest[4][4]);
};


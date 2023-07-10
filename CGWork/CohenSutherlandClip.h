#pragma once


class CohenSutherlandClip
{
private:
	int top = 0;
	int bottom = 0;
	int left = 0;
	int right = 0;
	int front = 0;
	int behind = 0;

	const int INSIDE = 0;
	const int LEFT = 1;
	const int RIGHT = 2;
	const int BOTTOM = 4;
	const int TOP = 8;
	const int FRONT = 16;
	const int BEHIND = 32;


public:
	void SetLimits(int top, int bottom, int right, int left, int front, int behind);
	bool Clip(int& x1, int& y1, double& z1, int& x2, int& y2, double& z2);
private:
	int calcCode(int x, int y, double z);
};

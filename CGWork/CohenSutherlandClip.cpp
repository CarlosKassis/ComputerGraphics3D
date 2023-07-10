#include "CohenSutherlandClip.h"


void CohenSutherlandClip::SetLimits(int top, int bottom, int right, int left, int front, int behind)
{
	this->top = top;
	this->bottom = bottom;
	this->right = right;
	this->left = left;
	this->front = front;
	this->behind = behind;
}

int CohenSutherlandClip::calcCode(int x, int y, double z)
{
	int code = INSIDE;
	if (x < left)
		code |= LEFT;
	else if (x > right)
		code |= RIGHT;
	if (y < top)
		code |= BOTTOM;
	else if (y > bottom)
		code |= TOP;
	if (z < front)
		code |= FRONT;
	else if (z > behind)
		code |= BEHIND;

	return code;
}



bool CohenSutherlandClip::Clip(int& x1, int& y1, double& z1, int& x2, int& y2, double& z2)
{
	int code1 = calcCode(x1, y1, z1);
	int code2 = calcCode(x2, y2, z2);
	bool accept = false;

	while (true) {
		if ((code1 == 0) && (code2 == 0)) {
			accept = true;
			break;
		}
		else if (code1 & code2) {
			break;
		}
		else {
			int code_out;
			int x, y, z;
			if (code1 != 0)
				code_out = code1;
			else
				code_out = code2;
			if (code_out & TOP) {
				x = x1 + (x2 - x1) * (bottom - y1) / (y2 - y1);
				z = z1 + (z2 - z1) * (bottom - y1) / (y2 - y1);
				y = bottom;
			}
			else if (code_out & BOTTOM) {
				x = x1 + (x2 - x1) * (top - y1) / (y2 - y1);
				z = z1 + (z2 - z1) * (top - y1) / (y2 - y1);
				y = top;
			}
			else if (code_out & RIGHT) {
				y = y1 + (y2 - y1) * (right - x1) / (x2 - x1);
				z = z1 + (z2 - z1) * (right - x1) / (x2 - x1);
				x = right;
			}
			else if (code_out & LEFT) {
				y = y1 + (y2 - y1) * (left - x1) / (x2 - x1);
				z = z1 + (z2 - z1) * (left - x1) / (x2 - x1);
				x = left;
			}
			else if (code_out & FRONT) {
				y = y1 + (y2 - y1) * (front - z1) / (z2 - z1);
				x = x1 + (x2 - x1) * (front - z1) / (z2 - z1);
				z = front;
			}
			else if (code_out & BEHIND) {
				y = y1 + (y2 - y1) * (behind - z1) / (z2 - z1);
				x = x1 + (x2 - x1) * (behind - z1) / (z2 - z1);
				z = behind;
			}
			if (code_out == code1) {
				x1 = x;
				y1 = y;
				z1 = z;
				code1 = calcCode(x1, y1, z1);
			}
			else {
				x2 = x;
				y2 = y;
				z2 = z;
				code2 = calcCode(x2, y2, z2);
			}
		}
	}
	return accept;
}

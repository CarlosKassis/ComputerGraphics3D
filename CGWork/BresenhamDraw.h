#pragma once
#include <afxwin.h>
#include "CohenSutherlandClip.h"
#include "ViewBuffer.h"
#include "Globals.h"

class BresenhamDraw
{
private:
	int m_nCurrentPosition[3] = { 0, 0, 0 };
	CohenSutherlandClip m_Clip;
	ViewBuffer* m_Buffer;
public:
	BresenhamDraw(ViewBuffer* buffer) : m_Buffer(buffer) {}

	void SetLimits(int top, int bottom, int right, int left, int front, int behind);

	void MoveTo(int x, int y, double z);
	void LineTo(int x, int y, double z, COLORREF color, GameObject& gameObject, bool force = false);

private:
	void DrawHigh(int x1, int y1, double z1, int x2, int y2, double z2, COLORREF color, GameObject& gameObject, bool force);
	void DrawLow(int x1, int y1, double z1, int x2, int y2, double z2, COLORREF color, GameObject& gameObject, bool force);

};

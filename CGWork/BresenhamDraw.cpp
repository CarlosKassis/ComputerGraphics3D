#include "BresenhamDraw.h"
#include "Globals.h"

void BresenhamDraw::SetLimits(int top, int bottom, int right, int left, int front, int behind)
{
	m_Clip.SetLimits(top, bottom, right, left, front, behind);
}

void BresenhamDraw::MoveTo(int x, int y, double z)
{
	m_nCurrentPosition[0] = x;
	m_nCurrentPosition[1] = y;
	m_nCurrentPosition[2] = z;

}

void BresenhamDraw::LineTo(int x, int y, double z, COLORREF color, GameObject& gameObject, bool force)
{
	int x1 = x, y1 = y;
	double z1 = z;
	int x2 = m_nCurrentPosition[0], y2 = m_nCurrentPosition[1];
	double z2 = m_nCurrentPosition[2];

	if (m_Clip.Clip(x1, y1, z1, x2, y2, z2))
	{
		if (abs(x1 - x2) > abs(y1 - y2))
		{
			if (x1 > x2)
			{
				DrawLow(x2, y2, z2, x1, y1, z1, color, gameObject, force);
			}
			else
			{
				DrawLow(x1, y1, z1, x2, y2, z2, color, gameObject, force);
			}
		}
		else
		{
			if (y1 > y2)
			{
				DrawHigh(x2, y2, z2, x1, y1, z1, color, gameObject, force);
			}
			else
			{
				DrawHigh(x1, y1, z1, x2, y2, z2, color, gameObject, force);
			}
		}
	}

	MoveTo(x, y, z);
}

void BresenhamDraw::DrawHigh(int x1, int y1, double z1, int x2, int y2, double z2, COLORREF color, GameObject& gameObject, bool force)
{
	double dz = ((double)(z2 - z1 + 1)) / (y2 - y1 + 1);

	int curr_x = x1;
	int curr_y = y1;
	double curr_z = z1;

	int dx = x2 - x1;
	const int dy = y2 - y1;
	int x_inc = 1;
	if (dx < 0)
	{
		x_inc = -1;
		dx = -dx;
	}

	int d = (2 * dx) - dy;
	const int de = 2 * dx;
	const int dne = 2 * (dx - dy);

	m_Buffer->SetPixel(curr_x, curr_y, curr_z, color, &gameObject);
	while (curr_y < y2)
	{
		if (d < 0)
		{
			d += de;
			curr_y += 1;
		}
		else
		{
			d += dne;
			curr_y += 1;
			curr_x += x_inc;
		}
		
		if (z1 != z2)
		{
			curr_z += dz;
		}

		m_Buffer->SetPixel(curr_x, curr_y, curr_z, color, &gameObject);
	}
}

void BresenhamDraw::DrawLow(int x1, int y1, double z1, int x2, int y2, double z2, COLORREF color, GameObject& gameObject, bool force)
{
	double dz = ((double)(z2 - z1 + 1)) / (x2 - x1 + 1);

	int curr_x = x1;
	int curr_y = y1;
	double curr_z = z1;
	const int dx = x2 - x1;
	int dy = y2 - y1;
	int y_inc = 1;
	if (dy < 0)
	{
		y_inc = -1;
		dy = -dy;
	}

	int d = (2 * dy) - dx;
	const int de = 2 * dy;
	const int dne = 2 * (dy - dx);

	m_Buffer->SetPixel(curr_x, curr_y, curr_z, color, &gameObject, force);
	while (curr_x < x2)
	{
		if (d < 0)
		{
			d += de;
			curr_x += 1;
		}
		else
		{
			d += dne;
			curr_x += 1;
			curr_y += y_inc;
		}

		if (z1 != z2)
		{
			curr_z += dz;
		}

		m_Buffer->SetPixel(curr_x, curr_y, curr_z, color, &gameObject, force);
	}
}

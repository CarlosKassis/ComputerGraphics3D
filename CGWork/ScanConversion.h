#pragma once

#include "Physics.h"
#include "GameObject.h"
#include <vector>
#include <cmath>
#include <fstream>
#include <algorithm>
#include <afxwin.h>
#include "CohenSutherlandClip.h"
#include "ViewBuffer.h"
#include "Globals.h"
#include "iritprsr.h"
#include "Camera.h"
#include "Light.h"

namespace ScanConversion {

	class ScanConversion
	{
	private:
		double m_front;
		double m_behind;
		ViewBuffer* m_Buffer;
		ShadingMode m_nMode;
		std::ofstream outfile;
	public:
		ScanConversion(ViewBuffer* buffer) : m_Buffer(buffer)
		{
		}

		~ScanConversion() = default;

		void SetShadingMode(ShadingMode shadingMode)
		{
			m_nMode = shadingMode;
		}

		void SetLimits(double front, double behind)
		{
			m_front = front;
			m_behind = behind;
		}

		struct XZ
		{
			double X;
			double Z;
			//Phong - save normal
			double NormalX;
			double NormalY;
			double NormalZ;
			//Gouraud - save color intensities
			double Red = 0.0;
			double Green = 0.0;
			double Blue = 0.0;

			bool operator < (const XZ& xz) const
			{
				return X < xz.X;
			}

			Vector3D Normal()
			{
				return Vector3D(NormalX, NormalY, NormalZ);
			}

			Vector3D PositionXZ()
			{
				return Vector3D(X, 0.0, Z);
			}

			Vector3D Color()
			{
				return Vector3D(Red, Green, Blue);
			}
		};

		double GetDeltaZ(double xRight, double zRight, double xLeft, double zLeft)
		{
			if (xRight == xLeft)
			{
				return min(zRight, zLeft); // Debatable
			}

			return (zRight - zLeft) / (xRight - xLeft);
		}

		void GetPolygonEdgesAndInfo(int viewHeight, std::vector<Vector3D> vertices, std::vector<Edge>& edges, int& polygonMinY, int& polygonMaxY)
		{
			polygonMinY = viewHeight;
			polygonMaxY = 0;
			for (int i = 0; i < vertices.size() - 1; i++)
			{
				Vector3D& v1 = vertices[i];
				polygonMaxY = max(polygonMaxY, v1.Y);
				polygonMinY = min(polygonMinY, v1.Y);
				edges.push_back(Edge(v1, vertices[i + 1]));
			}

			Vector3D& last = vertices[vertices.size() - 1];
			polygonMaxY = max(polygonMaxY, last.Y);
			polygonMinY = min(polygonMinY, last.Y);
			edges.push_back(Edge(last, vertices[0]));
		}

		Vector3D Interpolate(Vector3D& left, double mLeft, Vector3D& right, double mRight)
		{
			return Vector3D(left.X * mLeft + right.X * mRight, left.Y * mLeft + right.Y * mRight, left.Z * mLeft + right.Z * mRight);
		}

		void DrawPolygon(Polygon3D& polygon, COLORREF color, Camera& camera, GameObject& gameObject)
		{
			std::vector<Vector3D> vertices;
			for (Vector3D& vertex : polygon.Vertices)
			{
				Vector3D vertexWorldPosition = gameObject.AbsoluteToWorld(vertex);
				Vector3D vertexScreenPosition = camera.WorldToScreen(vertexWorldPosition);
				Vector3D vertexWorldNormal = gameObject.GetWithMyRotation(vertex.Normal());
				vertices.push_back(Vector3D(vertexScreenPosition, vertexWorldNormal, vertex.TypeOfNormal));
			}

			COLORREF lightRGB = RGB(0, 0, 0); // default black

			std::vector<Edge> A;
			int polygonMinY, polygonMaxY;
			GetPolygonEdgesAndInfo(m_Buffer->Height(), vertices, A, polygonMinY, polygonMaxY);

			for (int y = polygonMinY - 1; y < polygonMaxY + 1; y++)
			{
				std::vector<XZ> intersectionsXZ;
				for (auto& edge : A)
				{
					if (edge.maxY() >= y && edge.minY() <= y)
					{
						double x = edge.XAtY(y);
						double z = edge.ZAtX(x);

						Vector3D light;
						double normal[3];
						edge.GetNormal(y, normal);
						if (m_nMode == ShadingMode::GOURAUD)
						{
							light = Light::GetIntensities(camera, Vector3D(normal), camera.ScreenToWorld(Vector3D(x, y, z)), m_nMode);
						}

						intersectionsXZ.push_back({ x, z, normal[0], normal[1], normal[2], light.X, light.Y, light.Z });
					}
				}

				std::sort(intersectionsXZ.begin(), intersectionsXZ.end());

				if (!intersectionsXZ.size() || intersectionsXZ.size() % 2 != 0)
				{
					continue;
				}

				for (int i = 0; i < intersectionsXZ.size(); i += 2)
				{
					// Clip whole line
					if (intersectionsXZ[i].Z < m_front && intersectionsXZ[i + 1].Z < m_front ||
						intersectionsXZ[i].Z > m_behind && intersectionsXZ[i + 1].Z > m_behind)
					{
						continue;
					}

					auto& intersectionLeft = intersectionsXZ[i], &intersectionRight = intersectionsXZ[i + 1];

					// Get slope of scan line
					double z = intersectionsXZ[i].Z;
					double dz = GetDeltaZ(intersectionsXZ[i + 1].X, intersectionsXZ[i + 1].Z, intersectionsXZ[i].X, intersectionsXZ[i].Z);

					for (int x = intersectionsXZ[i].X; x < intersectionsXZ[i + 1].X; x++)
					{
						// Clip pixel
						if (z < m_front || z > m_behind)
						{
							z += dz;
							continue;
						}

						// Calc ratio
						double t = (intersectionRight.X - x) / (intersectionRight.X - intersectionLeft.X);

						if (m_nMode == ShadingMode::FLAT)
						{
							Vector3D polygonWorldNormal = gameObject.GetWithMyRotation(polygon.Normal());
							Vector3D pointPosition = Interpolate(intersectionLeft.PositionXZ(), t, intersectionRight.PositionXZ(), (1.0 - t));
							pointPosition.Y = y;

							Vector3D light = Light::GetIntensities(camera, polygonWorldNormal, camera.ScreenToWorld(pointPosition), m_nMode);
							lightRGB = RGB(min(255, GetRValue(color) * light.X), min(255, GetGValue(color) * light.Y), min(255, GetBValue(color) * light.Z));
						}
						else if (m_nMode == ShadingMode::GOURAUD)
						{
							Vector3D light = Interpolate(intersectionLeft.Color(), t, intersectionRight.Color(), 1.0 - t);
							lightRGB = RGB(min(255, GetRValue(color) * light.X), min(255, GetGValue(color) * light.Y), min(255, GetBValue(color) * light.Z));
						}
						else if (m_nMode == ShadingMode::PHONG)
						{
							Vector3D pointNormal = Interpolate(intersectionLeft.Normal(), t, intersectionRight.Normal(), (1.0 - t));
							Vector3D pointPosition = Interpolate(intersectionLeft.PositionXZ(), t, intersectionRight.PositionXZ(), (1.0 - t));
							pointPosition.Y = y;

							Vector3D light = Light::GetIntensities(camera, pointNormal, camera.ScreenToWorld(pointPosition), m_nMode);
							lightRGB = RGB(min(255, GetRValue(color) * light.X), min(255, GetGValue(color) * light.Y), min(255, GetBValue(color) * light.Z));
						}

						// Multiply by polygon color
						Vector3D polygonWithLightColor = Vector3D(lightRGB) * (polygon.Color * 255.0);
						COLORREF finalRGB = RGB(polygonWithLightColor.X, polygonWithLightColor.Y, polygonWithLightColor.Z);

						m_Buffer->SetPixel(x, y, z, finalRGB, &gameObject, false, polygon.Opacity);
						z += dz;
					}
				}
			}
		}
	};
}


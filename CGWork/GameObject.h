#pragma once

#include <vector>
#include <memory>
#include <tuple>
#include "symb_lib.h"
#include "iritprsr.h"
#include "MatUtils.h"
#include "Globals.h"
#include "Physics.h"

class Polygon3D
{
private:
	bool m_bCalculatedNormalInverted = false;
public:
	Vector3D Center;
	Vector3D Color = RGB(255, 255, 255);
	std::vector<Vector3D> Vertices;
	Vector3D CalculatedNormal;
	Vector3D ImportedNormal;
	NormalType NormalType = NormalType::Calculated;
	double Opacity = 1.0;

	void SetCalculatedNormal(Vector3D& calculatedNormal)
	{
		CalculatedNormal = calculatedNormal;
	}

	void SetImportedNormal(Vector3D& importedNormal)
	{
		ImportedNormal = importedNormal;
	}

	void InvertCalculatedNormal()
	{
		CalculatedNormal = -CalculatedNormal;
		m_bCalculatedNormalInverted = !m_bCalculatedNormalInverted;
	}

	bool IsCalculatedNormalInverted()
	{
		return m_bCalculatedNormalInverted;
	}

	Vector3D& Normal()
	{
		return NormalType == NormalType::Calculated ? CalculatedNormal : ImportedNormal;
	}
};

class GameObject
{
private:
	bool m_bTransMatrixDirty = true;

	double ScaleMatrix[4][4] =			IDENTITY4X4;
	double RotationMatrix[4][4] =		IDENTITY4X4;
	double PositionMatrix[4][4] =		IDENTITY4X4;

	double WorldRotationMatrix[4][4] =	IDENTITY4X4;

	double TransMatrix[4][4] =			IDENTITY4X4;
	double WorldTransMatrix[4][4] =		IDENTITY4X4;

	bool m_bInverseNormals = false;
	bool m_bCalculatedNormals = false;

	void UpdateWorldTransformation();

public:
	~GameObject() = default;
	bool Selected = false;
	bool MinMaxVertexCoordSet = false;
	double MinVertexCoord[3] = { 0 };
	double MaxVertexCoord[3] = { 0 };
	std::vector<Polygon3D> Polygons;

	void SetInitialScale(int axis, double scale);
	void AddScale(int axis, double scaleOffset, Space space);
	void Rotate(int axis, double angle, Space space);
	void Translate(int axis, double offset, Space space);
	void ResetTransformations();

	Vector3D AbsoluteToWorld(Vector3D& vector);
	Vector3D GetWithMyRotation(Vector3D& vector);

	void GetWorldTransformation(const GameObject& otherGameObject)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				ScaleMatrix[i][j] = otherGameObject.ScaleMatrix[i][j];
				RotationMatrix[i][j] = otherGameObject.RotationMatrix[i][j];
				PositionMatrix[i][j] = otherGameObject.PositionMatrix[i][j];

				WorldRotationMatrix[i][j] = otherGameObject.WorldRotationMatrix[i][j];

				TransMatrix[i][j] = otherGameObject.TransMatrix[i][j];
				WorldTransMatrix[i][j] = otherGameObject.WorldTransMatrix[i][j];
			}
		}
	}

	void AddTransparency(double offset) {

		for (auto& polygon : Polygons) {
			polygon.Opacity += offset;
			polygon.Opacity = min(1.0, max(0.0, polygon.Opacity));
		}
	}

	void UseCalculatedNormals()
	{
		for (Polygon3D& polygon : Polygons)
		{
			polygon.NormalType = NormalType::Calculated;
			for (Vector3D& vertex : polygon.Vertices)
			{
				vertex.TypeOfNormal = NormalType::Calculated;
			}
		}
	}

	void UseImportedNormals()
	{
		for (Polygon3D& polygon : Polygons)
		{
			polygon.NormalType = NormalType::Imported;
			for (Vector3D& vertex : polygon.Vertices)
			{
				vertex.TypeOfNormal = NormalType::Imported;
			}
		}
	}

	void InvertCalculatedNormals()
	{
		for (Polygon3D& polygon : Polygons)
		{
			polygon.InvertCalculatedNormal();
			for (Vector3D& vertex : polygon.Vertices)
			{
				vertex.InvertCalculatedNormal();
			}
		}
	}

	void CalculateVertexCalculatedNormals()
	{
		std::vector<std::tuple<Vector3D, std::vector<Vector3D>>> pointsNormals;

		for (Polygon3D& polygon : Polygons)
		{
			for (Vector3D& vertex : polygon.Vertices)
			{
				Vector3D rounded = vertex.Round3();
				bool present = false;

				// Dictionary doesn't work, O(n) it is
				for (auto& pointNormals : pointsNormals)
				{
					if (std::get<0>(pointNormals) == rounded)
					{
						auto& normals = std::get<1>(pointNormals);
						normals.push_back(polygon.CalculatedNormal);
						present = true;
						break;
					}
				}

				if (!present)
				{
					std::vector<Vector3D> normals;
					normals.push_back(polygon.CalculatedNormal);
					pointsNormals.push_back(std::make_tuple(rounded, normals));
				}
			}
		}

		// Calculate vertex normal as average of plane normals the vertex belongs to
		for (Polygon3D& polygon : Polygons)
		{
			for (Vector3D& vertex : polygon.Vertices)
			{
				Vector3D rounded = vertex.Round3();
				for (auto& pointNormals : pointsNormals)
				{
					if (std::get<0>(pointNormals) == rounded)
					{
						Vector3D average;
						auto& normals = std::get<1>(pointNormals);
						for (auto& normal : normals)
						{
							average += normal;
						}

						average /= normals.size() == 0 ? 1 : normals.size();

						vertex.SetNormal(average, NormalType::Calculated);
						break;
					}
				}
			}
		}
	}
};
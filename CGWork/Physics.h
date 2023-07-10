#pragma once

#include <afxwin.h>
#include <vector>
#include "Light.h"
#include "Globals.h"
#include "Camera.h"
#include "Resource.h"
#include "MatUtils.h"
#include "misc_lib.h"

class Vector3D {
private:
	double Round3Num(double num)
	{
		return ((int)(num * 1000.0)) / 1000.0;
	}

public:
	double X = 0.0;
	double Y = 0.0;
	double Z = 0.0;
	double CalculatedNormalX = 0.0;
	double CalculatedNormalY = 0.0;
	double CalculatedNormalZ = 0.0;
	double ImportedNormalX = 0.0;
	double ImportedNormalY = 0.0;
	double ImportedNormalZ = 0.0;
	NormalType TypeOfNormal = NormalType::Calculated;
	COLORREF m_nColor;
	Vector3D() = default;
	Vector3D(double x, double y, double z) : X(x), Y(y), Z(z) {}
	Vector3D(Vector3D& position, Vector3D& normal, NormalType typeOfNormal) : X(position.X), Y(position.Y), Z(position.Z)
	{
		SetNormal(normal, typeOfNormal);
	}

	Vector3D(double* array3D) : X(array3D[X_AXIS]), Y(array3D[Y_AXIS]), Z(array3D[Z_AXIS]) {}

	/// <summary>
	/// Create Vector3D from color RGB with values between [0 - 1]
	/// </summary>
	/// <param name="color"></param>
	Vector3D(COLORREF color) : X(GetRValue(color) / 255.0), Y(GetGValue(color) / 255.0), Z(GetBValue(color) / 255.0)
	{
		m_nColor = color;
	}

	void ToArray3D(double array[3]);
	void ToArray4D(double array[4]);

	Vector3D Round3()
	{
		return Vector3D(Round3Num(X), Round3Num(Y), Round3Num(Z));
	}

	Vector3D Normal()
	{
		return TypeOfNormal == NormalType::Calculated ? Vector3D(CalculatedNormalX, CalculatedNormalY, CalculatedNormalZ) : Vector3D(ImportedNormalX, ImportedNormalY, ImportedNormalZ);
	}

	void InvertCalculatedNormal()
	{
		CalculatedNormalX = -CalculatedNormalX;
		CalculatedNormalY = -CalculatedNormalY;
		CalculatedNormalZ = -CalculatedNormalZ;
	}

	void SetNormal(Vector3D& normal, NormalType typeOfNormal)
	{
		TypeOfNormal = typeOfNormal;
		if (typeOfNormal == NormalType::Calculated)
		{
			CalculatedNormalX = normal.X;
			CalculatedNormalY = normal.Y;
			CalculatedNormalZ = normal.Z;
		}
		else
		{
			ImportedNormalX = normal.X;
			ImportedNormalY = normal.Y;
			ImportedNormalZ = normal.Z;
		}
	}

	COLORREF ColorRGB()
	{
		return m_nColor;
	}

	double Magnitude() const;
	Vector3D& Normalize();
	static double DotProduct(const Vector3D a, const Vector3D b);
	static Vector3D CrossProduct(const Vector3D& v1, const Vector3D& v2);
	static Vector3D PlaneNormal(std::vector<Vector3D>& vertices);
	Vector3D operator* (double rhs);
	Vector3D operator/ (double rhs);
	Vector3D operator- (Vector3D& rhs);
	Vector3D operator+ (Vector3D& rhs);
	Vector3D operator* (Vector3D& rhs);
	Vector3D& operator*= (double rhs);
	Vector3D& operator/= (double rhs);
	Vector3D& operator+= (Vector3D& rhs);
	Vector3D& operator-= (Vector3D& rhs);
	Vector3D& operator-=(double rhs);
	Vector3D& operator+=(double rhs);
	Vector3D& operator*=(Vector3D& rhs);
	Vector3D operator-();
	double operator[](int index) const;
	bool operator==(const Vector3D& rhs);
};

class Camera
{
private:
	int m_nMode = ID_VIEW_ORTHOGRAPHIC;
	double m_dFar = MAX_FAR_CLIP;
	double m_dClose = MIN_CLOSE_CLIP;
	double m_dD = DEFAULT_D;
	double m_dOrthographicSize = DEFAULT_ORTHOGRAPHIC_SIZE;
	int m_nHeight = 1080.0;
	int m_nWidth = 1920.0;
	bool m_screenTransformationDirty = true;

	void WorldToView(double point[4]) const
	{
		// Currently support simple camera position subtraction
		for (int i = 0; i < 3; i++)
		{
			point[i] -= Position[i];
		}

		if (m_nMode == ID_VIEW_PERSPECTIVE)
		{
			double scaleByDistance = m_dD * point[Z_AXIS] / (MIN_D * D_DIVIDER);
			if (scaleByDistance != 0.0)
			{
				point[X_AXIS] /= scaleByDistance;
				point[Y_AXIS] /= scaleByDistance;
			}
		}
		else
		{
			if (m_dOrthographicSize != 0.0)
			{
				point[X_AXIS] /= m_dOrthographicSize;
				point[Y_AXIS] /= m_dOrthographicSize;
			}
		}
	}

	void ViewToWorld(double point[4]) const
	{
		if (m_nMode == ID_VIEW_PERSPECTIVE)
		{
			double scaleByDistance = m_dD * point[Z_AXIS] / (MIN_D * D_DIVIDER);
			point[X_AXIS] *= scaleByDistance;
			point[Y_AXIS] *= scaleByDistance;
		}
		else
		{
			point[X_AXIS] *= m_dOrthographicSize;
			point[Y_AXIS] *= m_dOrthographicSize;
		}

		// Currently support simple camera position subtraction
		for (int i = 0; i < 3; i++)
		{
			point[i] += Position[i];
		}
	}

	void ViewToScreen(double point[4]) const
	{
		// Screen space transformation
		point[0] = point[0] * m_nHeight + m_nWidth / 2.0;
		point[1] = point[1] * (-m_nHeight) + m_nHeight / 2.0;
	}

	void ScreenToView(double point[4]) const
	{
		// Screen space transformation
		point[0] = (point[0] - m_nWidth / 2.0) / (m_nHeight);
		point[1] = (point[0] - m_nHeight / 2.0) / (-m_nHeight);
	}

public:

	Vector3D Direction;
	Vector3D Position;

	Camera() : Direction(0.0, 0.0, 1.0), Position(0.0, 0.0, -12.0)
	{
	}

	void SetDimensions(int height, int width)
	{
		if (m_nHeight != height || m_nWidth != width)
		{
			m_nHeight = height;
			m_nWidth = width;
		}
	}

	void SetMode(int mode)
	{
		m_nMode = mode;
	}

	void SetClose(double closeClip)
	{
		m_dClose = max(1.0, closeClip);
	}

	void SetFar(double farClip)
	{
		m_dFar = farClip;
	}

	void SetD(double d)
	{
		m_dD = d;
	}

	void SetOrthographicSize(double size)
	{
		m_dOrthographicSize = size;
	}

	double GetOrthographicSize() const
	{
		return m_dOrthographicSize;
	}

	double GetFar() const
	{
		return m_dFar;
	}

	double GetClose() const
	{
		return m_dClose;
	}

	double GetD() const
	{
		return m_dD;
	}

	Vector3D WorldToScreen(Vector3D& vector) const
	{
		double vector4D[4];
		vector.ToArray4D(vector4D);
		WorldToView(vector4D);
		ViewToScreen(vector4D);
		return Vector3D(vector4D);
	}

	Vector3D ScreenToWorld(Vector3D& vector) const
	{
		double vector4D[4];
		vector.ToArray4D(vector4D);
		ScreenToView(vector4D);
		ViewToWorld(vector4D);
		return Vector3D(vector4D);
	}

	int GetInverseMatrix(double M[4][4], double InvM[4][4])
	{
		return MatInverseMatrix(M, InvM);
	}
};



class Edge {
private:
	Vector3D m_p1;
	Vector3D m_p2;
	double m_dMY;
	double m_dBY;

	double m_dMZ;
	double m_dBZ;
public:
	Edge(Vector3D point1, Vector3D point2) : m_p1(point1), m_p2(point2)
	{
		Vector3D& left = m_p1.X < m_p2.X ? m_p1 : m_p2;
		Vector3D& right = m_p1.X > m_p2.X ? m_p1 : m_p2;

		m_dMY = right.X == left.X ? 0.0 : ((double)(right.Y - left.Y) / (right.X - left.X));
		m_dBY = right.X == left.X ? 0.0 : (left.Y - m_dMY * left.X);

		m_dMZ = right.X == left.X ? 0.0 : ((double)(right.Z - left.Z) / (right.X - left.X));
		m_dBZ = right.X == left.X ? 0.0 : (left.Z - m_dMZ * left.X);
	}

	double minY() { return m_p1.Y < m_p2.Y ? m_p1.Y : m_p2.Y; }
	double maxY() { return m_p1.Y > m_p2.Y ? m_p1.Y : m_p2.Y; }

	// Get X from inverse Y line function calculated from two points
	double XAtY(double y);
	// Get Z from line Z function calculated from two points
	double ZAtX(double x);
	bool operator<(Edge& other);
	void GetNormal(double y, double out[3]);
};


class Light {
private:
	static double m_dAmbientIntensity;
	static double m_dDiffuseIntensity;
	static double m_dSpecularIntensity;
	static int m_nSpecularExponent;
	static COLORREF m_cAmbientColor;
	static Light* lights;

public:
	LightType Type;
	Vector3D Position;
	Vector3D DirectionNormalized;
	double Intensity = 0.0;
	bool Enabled = false;
	double SpotlightDotProductMin;
	COLORREF Color = RGB(255, 255, 255);

	Light() = default;
	Light(Vector3D position, Vector3D direction, double intensity = 1.0, COLORREF color = RGB(255, 255, 255), LightType type = LightType::LIGHT_TYPE_DIRECTIONAL)
		: Position(position), DirectionNormalized(direction.Normalize()), Color(color), Intensity(intensity), Type(type)
	{
	}

	Light(LightParams params);

	void SetSpotlightAngle(double angle)
	{
		SpotlightDotProductMin = cos(angle * M_PI / 180.0);
	}

	static Light* GetLightsInstance();
	static void DestroyLightsInstance();
	static void SetLight(int id, Light light);

	//Returns array of reflected colour intensities [R,G,B] for diffuse reflections
	static Vector3D GetIntensities(Camera& camera, Vector3D& pointNormal, Vector3D& point, ShadingMode type);

	static void SetRatios(double ambient, COLORREF ambientColor, double diffuse, double specular, int exponent);
};

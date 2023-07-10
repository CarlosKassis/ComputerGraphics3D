#include "Physics.h"

void Vector3D::ToArray3D(double *array3D)
{
	array3D[0] = X;
	array3D[1] = Y;
	array3D[2] = Z;
}

void Vector3D::ToArray4D(double *array4D)
{
	ToArray3D(array4D);
	array4D[3] = 1.0;
}

double Vector3D::Magnitude() const
{
	return sqrt(X * X + Y * Y + Z * Z);
}

// Normalize pIn vector into pOut
Vector3D& Vector3D::Normalize()
{
	if (X == 0 && Y == 0 && Z == 0)
	{
		return *this;
	}

	double magnitude = Magnitude();
	X = X / magnitude;
	Y = Y / magnitude;
	Z = Z / magnitude;
	return *this;
}

double Vector3D::DotProduct(const Vector3D a, const Vector3D b)
{
	return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
}

Vector3D Vector3D::CrossProduct(const Vector3D& v1, const Vector3D& v2)
{
	Vector3D crossProduct(v1.Y * v2.Z - v1.Z * v2.Y, v1.Z * v2.X - v1.X * v2.Z, v1.X * v2.Y - v1.Y * v2.X);
	return crossProduct.Normalize();
}

Vector3D Vector3D::PlaneNormal(std::vector<Vector3D>& vertices)
{
	return CrossProduct(vertices[1] - vertices[0], vertices[2] - vertices[1]).Normalize();
}

Vector3D Vector3D::operator*(double rhs)
{
	Vector3D out = *this;
	out.X = X * rhs;
	out.Y = Y * rhs;
	out.Z = Z * rhs;
	return out;
}

Vector3D Vector3D::operator/(double rhs)
{
	Vector3D out = *this;
	out.X = X / rhs;
	out.Y = Y / rhs;
	out.Z = Z / rhs;
	return out;
}

Vector3D Vector3D::operator-(Vector3D& rhs)
{
	Vector3D out = *this;
	out.X = X - rhs.X;
	out.Y = Y - rhs.Y;
	out.Z = Z - rhs.Z;
	return out;
}

Vector3D& Vector3D::operator*=(double rhs)
{
	X *= rhs;
	Y *= rhs;
	Z *= rhs;
	return *this;
}

Vector3D& Vector3D::operator/=(double rhs)
{
	X /= rhs;
	Y /= rhs;
	Z /= rhs;
	return *this;
}

Vector3D& Vector3D::operator+=(Vector3D& rhs)
{
	X += rhs.X;
	Y += rhs.Y;
	Z += rhs.Z;
	return *this;
}

Vector3D& Vector3D::operator*=(Vector3D& rhs)
{
	X *= rhs.X;
	Y *= rhs.Y;
	Z *= rhs.Z;
	return *this;
}

Vector3D& Vector3D::operator-=(Vector3D& rhs)
{
	X -= rhs.X;
	Y -= rhs.Y;
	Z -= rhs.Z;
	return *this;
}

Vector3D& Vector3D::operator-=(double rhs)
{
	X -= rhs;
	Y -= rhs;
	Z -= rhs;
	return *this;
}

Vector3D& Vector3D::operator+=(double rhs)
{
	X += rhs;
	Y += rhs;
	Z += rhs;
	return *this;
}

Vector3D Vector3D::operator-()
{
	Vector3D out = *this;
	out.X = -X;
	out.Y = -Y;
	out.Z = -Z;
	return out;
}

double Vector3D::operator[](int index) const
{
	return index == 0 ? X : (index == 1 ? Y : Z);
}

Vector3D Vector3D::operator+(Vector3D& rhs)
{
	Vector3D out = *this;
	out.X = X + rhs.X;
	out.Y = Y + rhs.Y;
	out.Z = Z + rhs.Z;
	return out;
}

Vector3D Vector3D::operator*(Vector3D& rhs)
{
	return Vector3D(X * rhs.X, Y * rhs.Y, Z * rhs.Z);
}

bool Vector3D::operator==(const Vector3D& rhs)
{
	return X == rhs.X && Y == rhs.Y && Z == rhs.Z;
}

double Edge::XAtY(double y)
{
	if (m_p1.X == m_p2.X)
	{
		return m_p1.X;
	}

	return (y - m_dBY) / m_dMY;
}

double Edge::ZAtX(double x)
{
	if (m_p1.X == m_p2.X)
	{
		// Get Z closest to us
		return min(m_p1.Z, m_p2.Z);
	}

	return  m_dMZ * x + m_dBZ;
}

bool Edge::operator<(Edge& other)
{
	return minY() < other.minY();
}

void Edge::GetNormal(double y, double out[3])
{
	//calc ratio
	double t = (y - m_p2.Y) / (m_p1.Y - m_p2.Y);
	//interpolate normal
	Vector3D normal1 = m_p1.Normal();
	Vector3D normal2 = m_p2.Normal();
	for (int i = 0; i < 3; i++)
	{
		out[i] = (normal1[i] * t) + (normal2[i] * (1 - t));
	}
}

//Init lights static members

Light* Light::lights = nullptr;
double Light::m_dAmbientIntensity = 0.2;
double Light::m_dDiffuseIntensity = 0.8;
double Light::m_dSpecularIntensity = 2;
int Light::m_nSpecularExponent = 2;
COLORREF Light::m_cAmbientColor = RGB(255, 255, 255);



Light::Light(LightParams params)
{
	Position = Vector3D(params.posX, params.posY, params.posZ);
	DirectionNormalized = Vector3D(params.dirX, params.dirY, params.dirZ);

	Enabled = params.enabled;
	Type = params.type;
	Intensity = 1.0;
	Color = RGB(params.colorR, params.colorG, params.colorB);
	SetSpotlightAngle(params.angle);
}

Light* Light::GetLightsInstance()
{
	if (lights == nullptr) {
		//default parameters
		m_dAmbientIntensity = 0.2;
		m_dDiffuseIntensity = 0.8;
		m_dSpecularIntensity = 2;
		m_nSpecularExponent = 2;
		m_cAmbientColor = RGB(255,255,255);
		//init array
		lights = new Light[MAX_LIGHT];
		
		for (int i = MAX_LIGHT; i < MAX_LIGHT; i++) {
			lights[i] = Light();
		}
	}
	return lights;
}

void Light::DestroyLightsInstance()
{
	delete[] lights;
}

void Light::SetLight(int id, Light light)
{
	if (id < 0 && id >= MAX_LIGHT) {
		return;
	}

	lights[id] = light;
}

#define MAX_POINT_LIGHT_DISTANCE_REACH 10.0

Vector3D Light::GetIntensities(Camera& camera, Vector3D& pointNormal, Vector3D& worldPoint, ShadingMode type)
{
	Vector3D totalLight;
	for (int i = LIGHT_ID_1; i < MAX_LIGHT; i++)
	{
		if (!lights[i].Enabled)
		{
			continue;
		}

		Vector3D light;
		Vector3D lightDirection;
		double lightIntensity = 1.0; // Default = 1.0 for directional light where light doesn't decay
		if (lights[i].Type == LightType::LIGHT_TYPE_DIRECTIONAL)
		{
			lightDirection = lights[i].DirectionNormalized;
		}
		else if (lights[i].Type == LightType::LIGHT_TYPE_SPOT)
		{
			lightDirection = worldPoint - lights[i].Position;
			lightDirection.Normalize();
			double dotProduct = Vector3D::DotProduct(lightDirection, lights[i].DirectionNormalized);
			if (dotProduct < lights[i].SpotlightDotProductMin)
			{
				continue;
			}

			//double distanceFromPointLight = (worldPoint - lights[i].Position).Magnitude();
			//lightIntensity = 1.0 - (max(0.0, min(MAX_POINT_LIGHT_DISTANCE_REACH, distanceFromPointLight))) / MAX_POINT_LIGHT_DISTANCE_REACH;
		}
		else if (lights[i].Type == LightType::LIGHT_TYPE_POINT)
		{
			Vector3D pointLightPosition = lights[i].Position;
			lightDirection = worldPoint - pointLightPosition;

			//double distanceFromPointLight = lightDirection.Magnitude();
			//lightIntensity = 1.0 - (max(0.0, min(MAX_POINT_LIGHT_DISTANCE_REACH, distanceFromPointLight))) / MAX_POINT_LIGHT_DISTANCE_REACH;
			lightDirection.Normalize();
		}

		//diffuse
		double diffuse = Vector3D::DotProduct(lightDirection, pointNormal);
		double amplitudeDiffuse = max(0, diffuse * m_dDiffuseIntensity);
		light += amplitudeDiffuse;


		//Vector3D reflection(((pointNormal * (2.0 * amplitudeDiffuse)) - lightDirection).Normalize());
		Vector3D reflection((lightDirection - (pointNormal * 2.0 * diffuse)).Normalize());

		double specular = Vector3D::DotProduct(reflection, Vector3D(0, 0, 1));
		specular = pow(specular, m_nSpecularExponent);

		double amplitudeSpecular = max(0, specular * m_dSpecularIntensity);
		light += amplitudeSpecular;
		

		//scale according to light 
		light *= Vector3D(lights[i].Color) * lights[i].Intensity;
		light *= lightIntensity;
		totalLight += light;
	}

	//add ambient light
	totalLight += Vector3D(m_cAmbientColor) * m_dAmbientIntensity;

	return totalLight;
}

void Light::SetRatios(double ambient, COLORREF ambientColor, double diffuse, double specular, int exponent)
{
	m_cAmbientColor = ambientColor;
	m_dAmbientIntensity = ambient;
	m_dDiffuseIntensity = diffuse;
	m_dSpecularIntensity = specular;
	m_nSpecularExponent = exponent;
}

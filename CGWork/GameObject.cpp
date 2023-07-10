#include "GameObject.h"

void GameObject::Rotate(int axis, double angle, Space space)
{
	if (angle == 0)
		return;

	double matrix[4][4] = { {0, 0, 0 , 0}, {0, 0, 0 , 0}, {0, 0, 0 , 0}, {0, 0, 0 , 0} };

	if (axis == 0)
	{
		matrix[0][0] = 1;
		matrix[1][1] = cos(angle);
		matrix[1][2] = -sin(angle);
		matrix[2][1] = sin(angle);
		matrix[2][2] = cos(angle);
		matrix[3][3] = 1;
	}
	else if (axis == 1)
	{
		matrix[0][0] = cos(angle);
		matrix[0][2] = sin(angle);
		matrix[1][1] = 1;
		matrix[2][0] = -sin(angle);
		matrix[2][2] = cos(angle);
		matrix[3][3] = 1;
	}
	else
	{
		matrix[0][0] = cos(angle);
		matrix[0][1] = -sin(angle);
		matrix[1][0] = sin(angle);
		matrix[1][1] = cos(angle);
		matrix[2][2] = 1;
		matrix[3][3] = 1;
	}

	if (space == Space::ObjectSpace)
	{
		MatUtils::MatMul4x4(RotationMatrix, matrix, RotationMatrix);
	}
	else
	{
		MatUtils::MatMul4x4(matrix, WorldRotationMatrix, WorldRotationMatrix);
		MatUtils::MatMul4x4(matrix, WorldTransMatrix, WorldTransMatrix);
	}

	m_bTransMatrixDirty = true;
}

void GameObject::SetInitialScale(int axis, double scale)
{
	ScaleMatrix[axis][axis] = scale;

	m_bTransMatrixDirty = true;
}


void GameObject::AddScale(int axis, double scaleOffset, Space space)
{
	if (scaleOffset == 0.0)
		return;

	scaleOffset *= 0.01;
	if (space == Space::ObjectSpace)
	{
		ScaleMatrix[axis][axis] += scaleOffset;
	}
	else
	{
		double identity[4][4] =
		{
			{ 1.0, 0.0, 0.0, 0.0 },
			{ 0.0, 1.0, 0.0, 0.0 },
			{ 0.0, 0.0, 1.0, 0.0 },
			{ 0.0, 0.0, 0.0, 1.0 }
		};

		identity[axis][axis] = 1.0 + scaleOffset;
		MatUtils::MatMul4x4(identity, WorldTransMatrix, WorldTransMatrix);
	}

	m_bTransMatrixDirty = true;
}

void GameObject::Translate(int axis, double offset, Space space)
{
	if (offset == 0)
		return;

	if (space == Space::ObjectSpace)
	{
		double translationVector[4] = { 0 };
		translationVector[axis] = offset;
		MatUtils::MatMul4x1(RotationMatrix, translationVector);
		for (int i = 0; i < 4; i++)
		{
			PositionMatrix[i][3] += translationVector[i];
		}
	}
	else
	{
		double identity[4][4] =
		{
			{ 1.0, 0.0, 0.0, 0.0 },
			{ 0.0, 1.0, 0.0, 0.0 },
			{ 0.0, 0.0, 1.0, 0.0 },
			{ 0.0, 0.0, 0.0, 1.0 }
		};
		identity[axis][3] = offset;
		MatUtils::MatMul4x4(identity, WorldTransMatrix, WorldTransMatrix);
	}

	m_bTransMatrixDirty = true;
}

void GameObject::ResetTransformations()
{
	double identity[4][4] = IDENTITY4X4;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			ScaleMatrix[i][j] = identity[i][j];
			RotationMatrix[i][j] = identity[i][j];
			PositionMatrix[i][j] = identity[i][j];
			WorldRotationMatrix[i][j] = identity[i][j];
			TransMatrix[i][j] = identity[i][j];
			WorldTransMatrix[i][j] = identity[i][j];
		}
	}

	m_bTransMatrixDirty = true;
}

Vector3D GameObject::GetWithMyRotation(Vector3D& vector)
{
	double vector4D[4];
	vector.ToArray4D(vector4D);
	MatUtils::MatMul4x1(RotationMatrix, vector4D);
	MatUtils::MatMul4x1(WorldRotationMatrix, vector4D);
	return Vector3D(vector4D);
}

void GameObject::UpdateWorldTransformation()
{
	double identity[4][4] =
	{
		{ 1.0, 0.0, 0.0, 0.0 },
		{ 0.0, 1.0, 0.0, 0.0 },
		{ 0.0, 0.0, 1.0, 0.0 },
		{ 0.0, 0.0, 0.0, 1.0 }
	};

	double worldIdentity[4][4] =
	{
		{ 1.0, 0.0, 0.0, 0.0 },
		{ 0.0, 1.0, 0.0, 0.0 },
		{ 0.0, 0.0, 1.0, 0.0 },
		{ 0.0, 0.0, 0.0, 1.0 }
	};

	MatUtils::MatMul4x4(ScaleMatrix, identity, identity);
	MatUtils::MatMul4x4(RotationMatrix, identity, identity);
	MatUtils::MatMul4x4(PositionMatrix, identity, identity);

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			TransMatrix[i][j] = identity[i][j];
		}
	}

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			TransMatrix[i][j] = identity[i][j];
		}
	}

	m_bTransMatrixDirty = false;
}

Vector3D GameObject::AbsoluteToWorld(Vector3D& vector)
{
	if (m_bTransMatrixDirty)
	{
		UpdateWorldTransformation();
	}

	double vector4D[4];
	vector.ToArray4D(vector4D);
	MatUtils::MatMul4x1(TransMatrix, vector4D);
	MatUtils::MatMul4x1(WorldTransMatrix, vector4D);
	return Vector3D(vector4D);
}

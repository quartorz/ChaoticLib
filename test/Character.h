#pragma once

#include "Aliases.h"

#include "Matrix.h"

#include <cmath>

float ToRad(float deg)
{
	return deg * 3.1415926535f / 180.f;
}

using Vector = Matrix<2, 1>;

class Transform: public Matrix<2, 3>{
public:
	template <class Element>
	Transform(const std::initializer_list<Element> &list): Matrix<2, 3>(list)
	{
	}
	Transform()
	{
	}
	Transform operator*(const Transform &t)
	{
		return Transform({
			(*this)[0][0] * t[0][0] + (*this)[0][1] * t[1][0],
			(*this)[0][0] * t[0][1] + (*this)[0][1] * t[1][1],
			(*this)[0][0] * t[0][2] + (*this)[0][1] * t[1][2] + (*this)[0][2],
			(*this)[1][0] * t[0][0] + (*this)[1][1] * t[1][0],
			(*this)[1][0] * t[0][1] + (*this)[1][1] * t[1][1],
			(*this)[1][0] * t[0][2] + (*this)[1][1] * t[1][2] + (*this)[1][2],
		});
	}
	Vector operator*(const Vector &v)
	{
		return Vector({
			(*this)[0][0] * v[0][0] + (*this)[0][1] * v[1][0] + (*this)[0][2],
			(*this)[1][0] * v[0][0] + (*this)[1][1] * v[1][0] + (*this)[1][2],
		});
	}
	static Transform Identity()
	{
		return Transform({
			1.f, 0.f, 0.f,
			0.f, 1.f, 0.f,
		});
	}
	static Transform Translation(float x, float y)
	{
		return Transform({
			1.f, 0.f, x,
			0.f, 1.f, y,
		});
	}
	static Transform Translation(Aliases::Point p)
	{
		return Translation(p.x, p.y);
	}
	// theta: x軸の正方向からy軸の正方向に向かう角度
	static Transform Rotation(float theta)
	{
		float cos = std::cos(theta);
		float sin = std::sin(theta);
		return Transform({
			cos, -sin, 0.f,
			sin, cos, 0.f,
		});
	}
};

class Character{
	float radius = 1.f;

	// 現在位置からの移動ベクトル
	Vector vector;

	Aliases::Point pos;
	Aliases::Size size;

public:
	Aliases::Point GetCenter() const
	{
		return pos + Aliases::Point(size.width / 2, size.height / 2);
	}

	void SetPosition(const Aliases::Point &p)
	{
		pos = p;
	}
	Aliases::Point GetPosition() const
	{
		return pos;
	}
	void SetSize(const Aliases::Size &s)
	{
		size = s;
	}
	Aliases::Size GetSize() const
	{
		return size;
	}

	void SetRadius(float r)
	{
		radius = r;
	}

	float GetRadius() const
	{
		return radius;
	}

	Aliases::Circle GetCircle() const
	{
		return Aliases::Circle(GetCenter(), radius);
	}

	bool IsColliding(const Character &c)
	{
		return GetCircle().IsColliding(c.GetCircle());
	}

	void Move()
	{
		SetPosition(Aliases::Point(pos.x + vector[0][0], pos.y + vector[1][0]));
	}
	void SetVector(const Vector &v)
	{
		vector = v;
	}
	Vector GetVector() const
	{
		return vector;
	}
};


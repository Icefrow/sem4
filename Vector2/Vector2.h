#pragma once
#include <iostream>

#define PI 3.14159265

class Vector2
{
public:
	float x, y;
	Vector2(float x1, float y1);
	Vector2();
	Vector2& operator = (const Vector2 &other);
	friend std::istream &operator >> (std::istream &stream, Vector2 &v);
	friend std::ostream &operator << (std::ostream &stream, const Vector2 &v);
	Vector2 operator + (const Vector2 &other) const;
	Vector2 operator - (const Vector2 &other) const;
	Vector2& operator += (const Vector2 &other);
	Vector2& operator -= (const Vector2 &other);
	float operator * (const Vector2& other) const;
	float operator ^ (const Vector2& other) const;
	Vector2 operator * (float k) const;
	friend Vector2 operator * (float k, const Vector2 &v);
	Vector2& operator *= (float k);
	Vector2 operator / (float k) const;
	Vector2& operator /= (float k);
	Vector2 norm() const;
	Vector2 perpendicular() const;
	float len() const;
	Vector2 operator-() const;
	Vector2& rotate(float angle);
	Vector2 getRotated(float angle) const;
};

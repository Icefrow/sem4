#include "Vector2.h"
#include <iostream>
#include <cmath>

Vector2::Vector2(float x1, float y1)
{
	x = x1;
	y = y1;
}

Vector2::Vector2()
{
	x = 0;
	y = 0;
}

Vector2& Vector2::operator = (const Vector2 &other)
{
	x = other.x;
	y = other.y;
	return *this;
}

std::ostream& operator << (std::ostream& stream, const Vector2& v)
{
	stream << v.x << " " << v.y;
	return stream;
}

std::istream& operator >> (std::istream& stream, Vector2& v)
{
	stream >> v.x >> v.y;
	return stream;
}

Vector2  Vector2::operator + (const Vector2 &other) const
{
	return Vector2(x + other.x, y + other.y);
}

Vector2  Vector2::operator - (const Vector2 &other) const
{
	return Vector2(x - other.x, y - other.y);
}

Vector2& Vector2::operator += (const Vector2 &other)
{
	x = x + other.x;
	y = y + other.y;
	return *this;
}

Vector2& Vector2::operator -= (const Vector2 &other)
{
	x -= other.x;
	y -= other.y;
	return *this;
}

float Vector2::operator * (const Vector2& other) const
{
	return (x*other.x + y*other.y);
}

float Vector2::operator ^ (const Vector2& other) const
{
	return (x*other.y - y*other.x);
}

Vector2 Vector2::operator * (float k) const
{
	return Vector2(x*k, y*k);
}

Vector2 operator * (float k, const Vector2 &v)
{
	return Vector2(v.x*k, v.y*k);
}

Vector2& Vector2::operator *= (float k)
{
	x *= k;
	y *= k;
	return *this;
}

Vector2 Vector2::operator / (float k) const
{
	return Vector2(x / k, y / k);
}

Vector2& Vector2::operator /= (float k)
{
	x /= k;
	y /= k;
	return *this;
}

Vector2 Vector2::norm() const
{
	return Vector2(x / sqrt(x*x + y*y), y / sqrt(x*x + y*y));
}

Vector2 Vector2::perpendicular() const
{
	return Vector2(y, -x);
}

float Vector2::len() const
{
	return sqrt(x*x + y*y);
}

Vector2 Vector2::operator-() const
{
	return Vector2(-x, -y);
}

Vector2& Vector2::rotate(float angle)
{
	float x1 = x, y1 = y;
	x = x1*cos(angle / 180 * PI) - y1*sin(angle / 180 * PI);
	y = y1*cos(angle / 180 * PI) + x1*sin(angle / 180 * PI);
	return *this;
}


Vector2 Vector2::getRotated(float angle) const
{
	return Vector2(x*cos(angle / 180 * PI) - y*sin(angle / 180 * PI),
					y*cos(angle / 180 * PI) + x*sin(angle / 180 * PI));
}

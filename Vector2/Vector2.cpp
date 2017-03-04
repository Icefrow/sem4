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

/*
вы реализовали оператор присваивания абсолютно верно.
но если вы его не напишете, то компилятор сгенерирует точно такой же код для него сам.
т.е. лучше его самим не реализовывать ...

оператор присвоения стоит реализовывать самостоятельно, если 
вы в конструкторе, например, выделяете память в куче с помощью оператора new.

тогда тот код, который сгенерирует компилятор, будет работать не так, как хочется:
ptr = other.ptr;

т.е. скопируется только указатель, а не сами данные.
*/
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
	/*
		x += other.x;
	*/
	x = x + other.x;
	y = y + other.y;
	return *this;
}

Vector2& Vector2::operator -= (const Vector2 &other)
{
	/*
		x -= other.x;
	*/
	x -= other.x;
	y -= other.y;
	return *this;
}

float Vector2::operator * (const Vector2& other) const
{
	/*
		пробелы вокруг бинарных операторов
	*/
	return (x*other.x + y*other.y);
}

float Vector2::operator ^ (const Vector2& other) const
{
	/*
		...пробелы здесь и дальше в по тексту
	*/
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
	/*
		вы уже реализовали выше всю математику поворота. просто переиспользуйте это код:
		return Vector2(x, y).rotate(angle);
	*/
	return Vector2(x*cos(angle / 180 * PI) - y*sin(angle / 180 * PI),
					y*cos(angle / 180 * PI) + x*sin(angle / 180 * PI));
}

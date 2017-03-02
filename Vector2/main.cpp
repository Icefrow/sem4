#include "Vector2.h"
#include<iostream>

int main()
{
	Vector2 a, b, a1, b1;
	float x;
	std::cin >> a;
	std::cin >> b;
	a1 = a;
	b1 = b;
	std::cout << a1 << std::endl;
	std::cout << b1 << std::endl;
	std::cout << a + b << std::endl;
	std::cout << a - b << std::endl;
	a1 += b;
	std::cout << a1 << std::endl;
	a1 = a;
	a1 -= b;
	std::cout << a1 << std::endl;
	a1 = a;
	x = a * b;
	std::cout << x << std::endl;
	x = a ^ b;
	std::cout << x << std::endl;
	std::cout << a * 2 << std::endl;
	std::cout << 2 * a << std::endl;
	a1 *= 2;
	std::cout << a1 << std::endl;
	a1 = a;
	a1 /= 2;
	std::cout << a1 << std::endl;
	a1 = a;
	std::cout << a / 2 << std::endl;
	std::cout << a.norm() << std::endl;
	std::cout << a.perpendicular() << std::endl;
	std::cout << a.len() << std::endl;
	std::cout << -a << std::endl;
	a1.rotate(90);
	std::cout << a1 << std::endl;
	a1 = a;
	a1 = a.getRotated(90);
	std::cout << a1 << std::endl;
}

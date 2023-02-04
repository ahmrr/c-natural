#include <iostream>

int main() {
	auto a = 10;
	auto b = 20;
	std::cout << a+b << std::endl;
	a += 5;
	std::cout << a << std::endl;
	b -= 10;
	std::cout << b << std::endl;
	a *= b;
	std::cout << a << std::endl;
	b = 20;
	a = 10;
	b /= a;
	std::cout << b << std::endl;
	std::cout << sizeof(a) << std::endl;
	return 0;
}

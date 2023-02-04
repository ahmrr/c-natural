#include <iostream>
#include <cmath>

int main() {
	auto x = 10;
	auto y = 9+x;
	auto z = x*y/2-6;
	auto comp2 = z!=x;
	x = 9;
	y = 4;
	int a;
	a = 5;
	int b;
	b = 7;
	std::cout << a << std::endl;
	std::cout << sizeof(a) << std::endl;
	if (x>y) {
		if (y>z) {
			std::cout << x << std::endl;
			std::cout << z << std::endl;
		}
		std::cout << y << std::endl;
		std::cout << x << std::endl;
	}
	std::cout << z << std::endl;
	std::cout << y << std::endl;
	for (int i = 0; i <= 9; i++) {
		std::cout << y << std::endl;
	}
	while (x<y) {
		std::cout << y << std::endl;
		y += x;
	}
	return 0;
}

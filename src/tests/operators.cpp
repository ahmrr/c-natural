#include <iostream>

int main() {
	auto x = 5;
	auto y = x+7-3;
	auto z = y/6*8%3;
	auto pointer = &x;
	x = *pointer;
	auto xy = x<y;
	return 0;
}

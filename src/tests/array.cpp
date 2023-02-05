#include <iostream>

int main() {
	auto x = 10;
	if (x==10) {
		int testArray[10];
		testArray[7] = 1503;
		x = testArray[7];
		for (int i = 0; i <= 9; i++) {
			testArray[i] = x;
			std::cout << y << std::endl;
		}
	}
	return 0;
}

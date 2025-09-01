#include "include/lib.h"

#include <println>

int main(int, char **) {
	std::println("Version: {}\nHello, world!", version());
	return 0;
}

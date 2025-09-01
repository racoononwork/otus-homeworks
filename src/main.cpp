#include "include/lib.h"

#include <print>

int main(int, char **) {
	std::println("Version: {}\nHello, world!", version());
	return 0;
}

#include "advent/advent_of_code.h"

#include <iostream>

int main()
{
	// Use the filter to only run certain tests.
	// This uses some magic to test against the name of the function,
	// so putting "eighteen" as the argument will only run advent_eighteen_p1()
	// and advent_eighteen_p2() (as well as any other test functions with "eighteen"
	// in the function name.
	// Leave blank to run everything.
	verify_all();
#ifndef WIN32
	std::cout << "Program finished. Press any key to continue.";
	std::cin.get();
#endif
	return 0;
}
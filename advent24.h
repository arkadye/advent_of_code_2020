#pragma once

#include "advent_types.h"

ResultType day_twentyfour_testcase_a();
ResultType day_twentyfour_testcase_b_generic(int num_iterations);

template <int ITERATIONS>
inline ResultType day_twentyfour_testcase_b()
{
	return day_twentyfour_testcase_b_generic(ITERATIONS);
}

ResultType advent_twentyfour_p1();
ResultType advent_twentyfour_p2();
#pragma once

using TestFunc = std::function<ResultType()>;

// This describes a test to run.
struct verification_test
{
	std::string name;
	TestFunc test_func;
	std::string expected_result;
	bool result_known;
};

// A type to use to indicate the result is not known yet. Using this in a verification test
// will run the test and report the result, but will count as neither pass nor failure.
struct Dummy {};

verification_test make_test(std::string name, TestFunc func, int64_t result);
verification_test make_test(std::string name, TestFunc func, std::string result);
verification_test make_test(std::string name, TestFunc func, Dummy);
verification_test make_test(std::string name, Dummy, Dummy);

#define ARG(func_name) std::string{ #func_name },func_name
#define TESTCASE(func_name,expected_result) make_test(ARG(func_name),expected_result)
#define FUNC_NAME(day_num,part_num) advent_ ## day_num ## _p ## part_num
#define TEST_DECL(day_num,part_num,expected_result) TESTCASE(FUNC_NAME(day_num,part_num),expected_result)
#define DAY(day_num,part1_result,part2_result) \
	TEST_DECL(day_num,1,part1_result), \
	TEST_DECL(day_num,2,part2_result)

static std::string DEFAULT_FILTER = "advent_";

static const verification_test tests[] =
{
	TESTCASE(day_one_testcase_a,514579),
	TESTCASE(day_one_testcase_b,241861950),
	DAY(one, Dummy{},Dummy{}),
	TESTCASE(day_two_p1_testcase,2),
	TESTCASE(day_two_p2_testcase,1),
	DAY(two,Dummy{},Dummy{}),
	TESTCASE(day_three_p1_testcase_a,7),
	TESTCASE(day_three_p2_testcase_a,336),
	DAY(three,Dummy{},Dummy{}),
	TESTCASE(day_four_p1_testcase_a,2),
	TESTCASE(day_four_p2_testcase_b,0),
	TESTCASE(day_four_p2_testcase_c,4),
	DAY(four,Dummy{},Dummy{}),
	TESTCASE(day_five_testcase_a,357),
	TESTCASE(day_five_testcase_b,567),
	TESTCASE(day_five_testcase_c,119),
	TESTCASE(day_five_testcase_d,820),
	DAY(five,Dummy{},Dummy{}),
	TESTCASE(day_six_testcase_a,11),
	TESTCASE(day_six_testcase_b,6),
	DAY(six,Dummy{},Dummy{}),
	TESTCASE(day_seven_testcase_a,4),
	TESTCASE(day_seven_testcase_b,32),
	TESTCASE(day_seven_testcase_c,126),
	DAY(seven,Dummy{},Dummy{}),
	TESTCASE(day_eight_testcase_a,5),
	TESTCASE(day_eight_testcase_b,8),
	DAY(eight,Dummy{},Dummy{}),
	TESTCASE(day_nine_testcase_a,127),
	TESTCASE(day_nine_testcase_b,62),
	DAY(nine,Dummy{},Dummy{}),
	TESTCASE(day_ten_testcase_a,35),
	TESTCASE(day_ten_testcase_b,220),
	TESTCASE(day_ten_testcase_c,8),
	TESTCASE(day_ten_testcase_d,19208),
	DAY(ten,Dummy{},Dummy{}),
	TESTCASE(day_eleven_testcase_a,37),
	TESTCASE(day_eleven_testcase_b,26),
	DAY(eleven,Dummy{},Dummy{}),
	TESTCASE(day_twelve_testcase_a,25),
	TESTCASE(day_twelve_testcase_b,286),
	DAY(twelve,Dummy{},Dummy{}),
	TESTCASE(day_thirteen_testcase_a,295),
	TESTCASE(day_thirteen_testcase_b,1068781),
	TESTCASE(day_thirteen_testcase_c,3417),
	TESTCASE(day_thirteen_testcase_d,754018),
	TESTCASE(day_thirteen_testcase_e,779210),
	TESTCASE(day_thirteen_testcase_f,1261476),
	TESTCASE(day_thirteen_testcase_g,1202161486),
	DAY(thirteen,Dummy{},Dummy{}),
	TESTCASE(day_fourteen_testcase_a,165),
	TESTCASE(day_fourteen_testcase_b,208),
	DAY(fourteen,Dummy{},Dummy{}),
	TESTCASE(day_fifteen_testcase_a,436),
	TESTCASE(day_fifteen_testcase_b,1),
	TESTCASE(day_fifteen_testcase_c,10),
	TESTCASE(day_fifteen_testcase_d,27),
	TESTCASE(day_fifteen_testcase_e,78),
	TESTCASE(day_fifteen_testcase_f,438),
	TESTCASE(day_fifteen_testcase_g,1836),
	TESTCASE(day_fifteen_testcase_h,175594),
	TESTCASE(day_fifteen_testcase_i,2578),
	TESTCASE(day_fifteen_testcase_j,3544142),
	TESTCASE(day_fifteen_testcase_k,261214),
	TESTCASE(day_fifteen_testcase_l,6895259),
	TESTCASE(day_fifteen_testcase_m,18),
	TESTCASE(day_fifteen_testcase_n,362),
	DAY(fifteen,Dummy{},Dummy{}),
	TESTCASE(day_sixteen_testcase_a,71),
	TESTCASE(day_sixteen_testcase_b,"class=12 row=11 seat=13"),
	DAY(sixteen,Dummy{},Dummy{}),
	TESTCASE(day_seventeen_testcase_a,112),
	TESTCASE(day_seventeen_testcase_b,848),
	DAY(seventeen,Dummy{},Dummy{}),
	TESTCASE(day_eighteen_testcase_a,71),
	TESTCASE(day_eighteen_testcase_b,51),
	TESTCASE(day_eighteen_testcase_c,26),
	TESTCASE(day_eighteen_testcase_d,437),
	TESTCASE(day_eighteen_testcase_e,12240),
	TESTCASE(day_eighteen_testcase_f,13632),
	TESTCASE(day_eighteen_testcase_g,231),
	TESTCASE(day_eighteen_testcase_h,51),
	TESTCASE(day_eighteen_testcase_i,46),
	TESTCASE(day_eighteen_testcase_j,1445),
	TESTCASE(day_eighteen_testcase_k,669060),
	TESTCASE(day_eighteen_testcase_l,23340),
	DAY(eighteen,Dummy{},Dummy{}),
	TESTCASE(day_nineteen_testcase_a,2),
	TESTCASE(day_nineteen_testcase_b,12),
	DAY(nineteen,Dummy{},Dummy{}),
	TESTCASE(day_twenty_testcase_a,20899048083289),
	TESTCASE(day_twenty_testcase_b,273),
	DAY(twenty,Dummy{},Dummy{}),
	TESTCASE(day_twentyone_testcase_a,5),
	TESTCASE(day_twentyone_testcase_b,"mxmxvkd,sqjhc,fvjkl"),
	DAY(twentyone,Dummy{},Dummy{}),
	TESTCASE(day_twentytwo_testcase_a,306),
	TESTCASE(day_twentytwo_testcase_b,291),
	DAY(twentytwo,Dummy{},Dummy{}),
	TESTCASE(day_twentythree_testcase_a,92658374),
	TESTCASE(day_twentythree_testcase_b,67384529),
	TESTCASE(day_twentythree_testcase_c, 149245887792),
	DAY(twentythree,Dummy{},Dummy{}),
	TESTCASE(day_twentyfour_testcase_a,10),
	TESTCASE(day_twentyfour_testcase_b<1>, 15),
	TESTCASE(day_twentyfour_testcase_b<2>, 12),
	TESTCASE(day_twentyfour_testcase_b<3>, 25),
	TESTCASE(day_twentyfour_testcase_b<4>, 14),
	TESTCASE(day_twentyfour_testcase_b<5>, 23),
	TESTCASE(day_twentyfour_testcase_b<6>, 28),
	TESTCASE(day_twentyfour_testcase_b<7>, 41),
	TESTCASE(day_twentyfour_testcase_b<8>, 37),
	TESTCASE(day_twentyfour_testcase_b<9>, 49),
	TESTCASE(day_twentyfour_testcase_b<10>, 37),
	TESTCASE(day_twentyfour_testcase_b<20>, 132),
	TESTCASE(day_twentyfour_testcase_b<30>, 259),
	TESTCASE(day_twentyfour_testcase_b<40>, 406),
	TESTCASE(day_twentyfour_testcase_b<50>, 566),
	TESTCASE(day_twentyfour_testcase_b<60>, 788),
	TESTCASE(day_twentyfour_testcase_b<70>, 1106),
	TESTCASE(day_twentyfour_testcase_b<80>, 1373),
	TESTCASE(day_twentyfour_testcase_b<90>, 1844),
	TESTCASE(day_twentyfour_testcase_b<100>, 2208),
	DAY(twentyfour,Dummy{},Dummy{}),
	TESTCASE(day_twentyfive_testcase_a, 14897079),
	DAY(twentyfive, Dummy{},"MERRY CHRISTMAS!")
};

#undef ARG
#undef TESTCASE
#undef FUNC_NAME
#undef TEST_DECL
#undef DAY
#undef DUMMY
#undef DUMMY_DAY

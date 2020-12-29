#include "../advent/advent12.h"
#include "../utils/Coords.h"
#include "../utils/advent_utils.h"

#include <optional>
#include <sstream>

namespace
{
	using namespace utils;
	enum class DirCommand : char
	{
		north = 'N',
		east = 'E',
		south = 'S',
		west = 'W',
		left = 'L',
		right = 'R',
		forward = 'F'
	};

	class Ship_p1
	{
		Coords position{ 0,0 };
		Direction direction = Direction::Right;
	public:
		void execute_command(DirCommand command, int arg)
		{
			const Coords offset = [command,this]()
			{
				switch (command)
				{
				case DirCommand::north:
					return Coords::up();
				case DirCommand::east:
					return Coords::right();
				case DirCommand::south:
					return Coords::down();
				case DirCommand::west:
					return Coords::left();
				case DirCommand::forward:
					return Coords::dir(direction);
				default:
					break;
				}
				return Coords{ 0,0 };
			}();

			const auto turn_cmd = [command]() -> std::optional<TurnDir>
			{
				switch (command)
				{
				case DirCommand::left:
					return TurnDir::Anticlockwise;
				case DirCommand::right:
					return TurnDir::Clockwise;
				default:
					break;
				}
				return std::nullopt;
			}();

			position += arg * offset;
			if (turn_cmd.has_value())
			{
				assert(arg % 90 == 0);
				while (arg > 0)
				{
					direction = rotate(direction, *turn_cmd);
					arg -= 90;
				}
			}
		}

		Coords get_position() const { return position; }
	};

	class Ship_p2
	{
		Coords position{ 0,0 };
		Coords waypoint{ 10,1 };
	public:
		void execute_command(DirCommand command, int arg)
		{
			const Coords waypoint_offset = [command, this]()
			{
				switch (command)
				{
				case DirCommand::north:
					return Coords::up();
				case DirCommand::east:
					return Coords::right();
				case DirCommand::south:
					return Coords::down();
				case DirCommand::west:
					return Coords::left();
				default:
					break;
				}
				return Coords{ 0,0 };
			}();

			const auto turn_cmd = [command]() -> std::optional<TurnDir>
			{
				switch (command)
				{
				case DirCommand::left:
					return TurnDir::Anticlockwise;
				case DirCommand::right:
					return TurnDir::Clockwise;
				default:
					break;
				}
				return std::nullopt;
			}();

			if (turn_cmd.has_value())
			{
				assert(arg % 90 == 0);
				assert(arg < 360);

				auto rotate_90 = [](const Coords& wp, TurnDir dir)
				{
					switch (dir)
					{
					case TurnDir::Anticlockwise:
						return Coords{ -wp.y,wp.x };
					case TurnDir::Clockwise:
						return Coords{ wp.y,-wp.x };
					default:
						assert(false);
						break;
					}
					return Coords{ 0,0 };
				};

				switch (arg)
				{
				case 90:
					waypoint = rotate_90(waypoint, *turn_cmd);
					break;
				case 180:
					waypoint = -1 * waypoint;
					break;
				case 270:
					waypoint = rotate_90(waypoint, *turn_cmd == TurnDir::Clockwise ? TurnDir::Anticlockwise : TurnDir::Clockwise);
					break;
				default:
					assert(false);
					break;
				}
			}

			waypoint += arg * waypoint_offset;
			if (command == DirCommand::forward)
			{
				position += arg * waypoint;
			}
		}

		Coords get_position() const { return position; }
	};

	struct Command
	{
		int arg;
		DirCommand cmd;
	};

	Command extract_command(std::istream& input)
	{
		std::string full_command;
		input >> full_command;
		const char cmd_char = full_command[0];
		assert(std::string{ "NESWLRF" }.find(cmd_char) != std::string::npos);
		const auto command = static_cast<DirCommand>(cmd_char);
		const auto arg = std::stoi(full_command.substr(1));
		return Command{ arg,command };
	}

	Coords run_course_p1(std::istream& path)
	{
		Ship_p1 ship;
		while (!path.eof())
		{
			const Command cmd = extract_command(path);
			ship.execute_command(cmd.cmd, cmd.arg);
		}
		return ship.get_position();
	}

	int solve_p1(std::istream& path)
	{
		return run_course_p1(path).manhatten_distance();
	}

	Coords run_course_p2(std::istream& path)
	{
		Ship_p2 ship;
		while (!path.eof())
		{
			const Command cmd = extract_command(path);
			ship.execute_command(cmd.cmd, cmd.arg);
		}
		return ship.get_position();
	}

	int solve_p2(std::istream& path)
	{
		return run_course_p2(path).manhatten_distance();
	}

	std::istringstream get_testcase_a_input()
	{
		return std::istringstream{
			"F10\n"
			"N3\n"
			"F7\n"
			"R90\n"
			"F11"
		};
	}
}

ResultType day_twelve_testcase_a()
{
	std::istringstream input = get_testcase_a_input();
	return solve_p1(input);
}

ResultType advent_twelve_p1()
{
	std::ifstream input = open_puzzle_input(12);
	return solve_p1(input);
}

ResultType day_twelve_testcase_b()
{
	std::istringstream input = get_testcase_a_input();
	return solve_p2(input);
}

ResultType advent_twelve_p2()
{
	std::ifstream input = open_puzzle_input(12);
	return solve_p2(input);
}
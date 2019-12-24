#include <algorithm>
#include <cassert>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "solver.hpp"

constexpr int DAY = 11;

namespace
{

static std::vector<int64_t> read_input(const char* s)
{
  std::vector<int64_t> ret;
  while (*s != '\0')
  {
    int64_t v;
    sscanf(s, "%lld", &v);
    ret.push_back(v);
    while (*s != ',' && *s != '\0') s++;
    if (*s == ',') s++;
  }
  return ret;
}

class intcode_machine
{
public:
  enum class execution_state
  {
    ready,
    awaiting_input,
    halted,
    error
  };

  intcode_machine(std::vector<int64_t> const& program) : program(program)
  {
  }

  execution_state get_state() const
  {
    return execution_state;
  }

  void push_input(int64_t value)
  {
    input.push(value);
  }

  bool pop_output(int64_t& value)
  {
    if (output.size() > 0)
    {
      value = output.front();
      output.pop();
      return true;
    }
    return false;
  }

  execution_state run()
  {
    if (execution_state != execution_state::ready && execution_state != execution_state::awaiting_input)
    {
      return execution_state;
    }

    while (ip < program.size())
    {
      int64_t opcode_value = program[ip] % 100;
      switch (opcode_value)
      {
        // Add
        case 1:
        {
          int64_t pos_a = arg_position(0);
          int64_t pos_b = arg_position(1);
          int64_t pos_c = arg_position(2);
          program[pos_c] = program[pos_a] + program[pos_b];
          ip += 4;
        }
        break;
        // Multiply
        case 2:
        {
          int64_t pos_a = arg_position(0);
          int64_t pos_b = arg_position(1);
          int64_t pos_c = arg_position(2);
          program[pos_c] = program[pos_a] * program[pos_b];
          ip += 4;
        }
        break;
        // Read input
        case 3:
        {
          if (input.size() > 0)
          {
            int64_t pos_a = arg_position(0);
            program[pos_a] = input.front();
            input.pop();
            ip += 2;
          }
          else
          {
            execution_state = execution_state::awaiting_input;
            return execution_state;
          }
        }
        break;
        // Write output
        case 4:
        {
          int64_t pos_a = arg_position(0);
          int64_t value = program[pos_a];
          output.push(value);
          ip += 2;
        }
        break;
        // Jump if true
        case 5:
        {
          int64_t pos_a = arg_position(0);
          int64_t pos_b = arg_position(1);
          ip = (program[pos_a] != 0) ? program[pos_b] : ip + 3;
        }
        break;
        // Jump if false
        case 6:
        {
          int64_t pos_a = arg_position(0);
          int64_t pos_b = arg_position(1);
          ip = (program[pos_a] == 0) ? program[pos_b] : ip + 3;
        }
        break;
        // Less than
        case 7:
        {
          int64_t pos_a = arg_position(0);
          int64_t pos_b = arg_position(1);
          int64_t pos_c = arg_position(2);
          program[pos_c] = program[pos_a] < program[pos_b] ? 1 : 0;
          ip += 4;
        }
        break;
        // Equals
        case 8:
        {
          int64_t pos_a = arg_position(0);
          int64_t pos_b = arg_position(1);
          int64_t pos_c = arg_position(2);
          program[pos_c] = program[pos_a] == program[pos_b] ? 1 : 0;
          ip += 4;
        }
        break;
        // Change relative base
        case 9:
        {
          int64_t pos_a = arg_position(0);
          relative_base += program[pos_a];
          ip += 2;
        }
        break;
        // Halt
        case 99:
        {
          execution_state = execution_state::halted;
          return execution_state;
        }
        break;
        default:
        {
          execution_state = execution_state::error;
          return execution_state;
        }
        break;
      }
    }
    execution_state = execution_state::error;
    return execution_state;
  }

private:
  enum class arg_mode
  {
    position = 0,
    immediate = 1,
    relative = 2,
    invalid = -1
  };

  arg_mode get_argmode(int64_t opcode, int64_t arg) const
  {
    assert(arg >= 0);
    int64_t pow = 100;
    while (arg > 0)
    {
      pow *= 10;
      arg--;
    }
    int64_t opmode_value = (opcode / pow) % 10;
    switch (opmode_value)
    {
      case 0: return arg_mode::position;
      case 1: return arg_mode::immediate;
      case 2: return arg_mode::relative;
    }
    return arg_mode::invalid;
  }

  int64_t arg_position(int64_t arg)
  {
    int64_t opcode = program[ip];
    arg_mode mode = get_argmode(opcode, arg);
    int64_t pos = -1;
    switch (mode)
    {
      case arg_mode::position:
      {
        pos = program[ip + arg + 1];
      }
      break;
      case arg_mode::immediate:
      {
        pos = ip + arg + 1;
      }
      break;
      case arg_mode::relative:
      {
        pos = program[ip + arg + 1] + relative_base;
      }
      break;
      case arg_mode::invalid:
      default:
      {
        return -1;
      }
    }
    if (pos + 1 > (int64_t)program.size())
    {
      program.resize(pos + 1, 0);
    }
    return pos;
  }

  std::vector<int64_t> program;
  std::queue<int64_t> input;
  std::queue<int64_t> output;
  execution_state execution_state = execution_state::ready;
  uint64_t ip = 0;
  int64_t relative_base = 0;
};

struct vec2
{
  int x;
  int y;
};

static bool operator==(vec2 a, vec2 b)
{
  return a.x == b.x && a.y == b.y;
}

struct vec2_hasher
{
  uint64_t operator()(vec2 v) const
  {
    return v.x + v.y;
  }
};

enum class direction
{
  north = 0,
  east = 1,
  south = 2,
  west = 3
};

static direction turn_cw(direction d)
{
  switch (d)
  {
    case direction::north: return direction::east;
    case direction::east: return direction::south;
    case direction::south: return direction::west;
    case direction::west: return direction::north;
  }
  assert(false);
  return direction::north;
}

static direction turn_ccw(direction d)
{
  switch (d)
  {
    case direction::north: return direction::west;
    case direction::east: return direction::north;
    case direction::south: return direction::east;
    case direction::west: return direction::south;
  }
  assert(false);
  return direction::north;
}

static vec2 dir_to_vec2(direction d)
{
  switch (d)
  {
    case direction::north: return vec2{ 0, 1 };
    case direction::east: return vec2{ 1, 0 };
    case direction::south: return vec2{ 0, -1 };
    case direction::west: return vec2{ -1, 0 };
  }
  assert(false);
  return vec2{ 0, 0 };
}

struct robot
{
  vec2 position;
  direction dir;
};

enum class color
{
  black = 0,
  white = 1
};
} // namespace

void solver<DAY, 1>::solve(const char* input, char* output)
{
  std::vector<int64_t> program = read_input(input);
  robot robot;
  robot.position = vec2{ 0, 0 };
  robot.dir = direction::north;
  intcode_machine machine(program);
  std::unordered_map<vec2, color, vec2_hasher> tile_colors;
  while (machine.get_state() != intcode_machine::execution_state::halted)
  {
    assert(machine.get_state() != intcode_machine::execution_state::error);
    if (tile_colors.find(robot.position) == tile_colors.end())
    {
      tile_colors.emplace(robot.position, color::black);
    }
    machine.push_input(tile_colors.at(robot.position) == color::black ? 0 : 1);
    auto new_state = machine.run();
    assert(new_state != intcode_machine::execution_state::error);
    {
      int64_t new_color_code;
      bool got_output = machine.pop_output(new_color_code);
      assert(got_output);
      assert(new_color_code == 0 || new_color_code == 1);
      tile_colors.at(robot.position) = new_color_code == 0 ? color::black : color::white;
    }
    {
      int64_t turn_dir;
      bool got_output = machine.pop_output(turn_dir);
      assert(got_output);
      assert(turn_dir == 0 || turn_dir == 1);
      robot.dir = turn_dir == 0 ? turn_ccw(robot.dir) : turn_cw(robot.dir);
    }
    {
      int64_t dummy_output;
      bool got_output = machine.pop_output(dummy_output);
      assert(got_output == false);
    }
    robot.position.x += dir_to_vec2(robot.dir).x;
    robot.position.y += dir_to_vec2(robot.dir).y;
  }
  sprintf(output, "%llu", tile_colors.size());
}

void solver<DAY, 2>::solve(const char* input, char* output)
{
  std::vector<int64_t> program = read_input(input);
  robot robot;
  robot.position = vec2{ 0, 0 };
  robot.dir = direction::north;
  intcode_machine machine(program);
  std::unordered_map<vec2, color, vec2_hasher> tile_colors;
  tile_colors.emplace(robot.position, color::white);
  while (machine.get_state() != intcode_machine::execution_state::halted)
  {
    assert(machine.get_state() != intcode_machine::execution_state::error);
    if (tile_colors.find(robot.position) == tile_colors.end())
    {
      tile_colors.emplace(robot.position, color::black);
    }
    machine.push_input(tile_colors.at(robot.position) == color::black ? 0 : 1);
    auto new_state = machine.run();
    assert(new_state != intcode_machine::execution_state::error);
    {
      int64_t new_color_code;
      bool got_output = machine.pop_output(new_color_code);
      assert(got_output);
      assert(new_color_code == 0 || new_color_code == 1);
      tile_colors.at(robot.position) = new_color_code == 0 ? color::black : color::white;
    }
    {
      int64_t turn_dir;
      bool got_output = machine.pop_output(turn_dir);
      assert(got_output);
      assert(turn_dir == 0 || turn_dir == 1);
      robot.dir = turn_dir == 0 ? turn_ccw(robot.dir) : turn_cw(robot.dir);
    }
    {
      int64_t dummy_output;
      bool got_output = machine.pop_output(dummy_output);
      assert(got_output == false);
    }
    robot.position.x += dir_to_vec2(robot.dir).x;
    robot.position.y += dir_to_vec2(robot.dir).y;
  }

  vec2 rect_min{ INT_MAX, INT_MAX }, rect_max{ INT_MIN, INT_MIN };
  for (auto tile : tile_colors)
  {
    vec2 pos = tile.first;
    rect_min.x = std::min(pos.x, rect_min.x);
    rect_min.y = std::min(pos.y, rect_min.y);
    rect_max.x = std::max(pos.x, rect_max.x);
    rect_max.y = std::max(pos.y, rect_max.y);
  }

  for (int y = rect_max.y; y >= rect_min.y; y--)
  {
    for (int x = rect_min.x; x <= rect_max.x; x++)
    {
      color c = (tile_colors.find(vec2{ x, y }) == tile_colors.end()) ? color::black : tile_colors.at(vec2{ x, y });
      *output++ = (c == color::black ? ' ' : '*');
    }
    *output++ = '\n';
  }
  *output = 0;
}

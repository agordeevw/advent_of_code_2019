#include <algorithm>
#include <cassert>
#include <vector>
#include <queue>

#include "solver.hpp"

constexpr int DAY = 9;

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
      value = output.back();
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
} // namespace

void solver<DAY, 1>::solve(const char* input, char* output)
{
  std::vector<int64_t> program = read_input(input);
  intcode_machine machine(program);
  machine.push_input(1);
  machine.run();
  int64_t ret = -1;
  bool has_ret = machine.pop_output(ret);
  assert(has_ret);
  sprintf(output, "%lld", ret);
}

void solver<DAY, 2>::solve(const char* input, char* output)
{
  std::vector<int64_t> program = read_input(input);
  intcode_machine machine(program);
  machine.push_input(2);
  machine.run();
  int64_t value = -1;
  while (machine.pop_output(value))
  {
    sprintf(output, "%lld ", value);
    output += strlen(output);
  }
}

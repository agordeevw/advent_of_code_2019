#include <cassert>

#include "common/intcode_machine.hpp"

intcode_machine::intcode_machine(intcode_program const & program) : program(program)
{
}

void intcode_machine::modify_program(uint64_t address, int64_t value)
{
  if (address < program.size())
  {
    program[address] = value;
  }
}

void intcode_machine::push_input(int64_t value)
{
  input.push(value);
}

std::pair<bool, int64_t> intcode_machine::pop_output()
{
  if (output.size() > 0)
  {
    int64_t value = output.front();
    output.pop();
    return {true, value};
  }
  return {false, 0};
}

intcode_machine::execution_state intcode_machine::single_step()
{
  if (state != execution_state::ready && state != execution_state::awaiting_input)
  {
    return state;
  }

  if (ip >= program.size())
  {
    state = execution_state::instr_ptr_out_of_bounds;
    return state;
  }

  uint64_t pos[4];
  auto fetch_arg_positions = [this, &pos](int num_args) -> bool
  {
    bool pos_valid = true;
    for (int i = 0; i < num_args; i++)
    {
      pos[i] = arg_position(i);
      if (pos[i] == (uint64_t)-1 || pos[i] >= program.size())
      {
        pos_valid = false;
      }
    }
    return pos_valid;
  };

  int64_t opcode_value = program[ip] % 100;
  switch (opcode_value)
  {
    // Add
    case 1:
    {
      if (fetch_arg_positions(3))
      {
        program[pos[2]] = program[pos[0]] + program[pos[1]];
        ip += 4;
      }
    }
    break;
    // Multiply
    case 2:
    {
      if (fetch_arg_positions(3))
      {
        program[pos[2]] = program[pos[0]] * program[pos[1]];
        ip += 4;
      }
    }
    break;
    // Read input
    case 3:
    {
      if (input.size() == 0)
      {
        state = execution_state::awaiting_input;
      }
      else if (fetch_arg_positions(1))
      {
        program[pos[0]] = input.front();
        input.pop();
        ip += 2;
      }
    }
    break;
    // Write output
    case 4:
    {
      if (fetch_arg_positions(1))
      {
        output.push(program[pos[0]]);
        ip += 2;
      }
    }
    break;
    // Jump if true
    case 5:
    {
      if (fetch_arg_positions(2))
      {
        ip = (program[pos[0]] != 0) ? program[pos[1]] : ip + 3;
      }
    }
    break;
    // Jump if false
    case 6:
    {
      if (fetch_arg_positions(2))
      {
        ip = (program[pos[0]] == 0) ? program[pos[1]] : ip + 3;
      }
    }
    break;
    // Less than
    case 7:
    {
      if (fetch_arg_positions(3))
      {
        program[pos[2]] = program[pos[0]] < program[pos[1]] ? 1 : 0;
        ip += 4;
      }
    }
    break;
    // Equals
    case 8:
    {
      if (fetch_arg_positions(3))
      {
        program[pos[2]] = program[pos[0]] == program[pos[1]] ? 1 : 0;
        ip += 4;
      }
    }
    break;
    // Change relative base
    case 9:
    {
      if (fetch_arg_positions(1))
      {
        relative_base += program[pos[0]];
        ip += 2;
      }
    }
    break;
    // Halt
    case 99:
    {
      state = execution_state::halted;
    }
    break;
    // Unknown code
    default:
    {
      state = execution_state::invalid_op;
    }
    break;
  }

  return state;
}

intcode_machine::execution_state intcode_machine::run()
{
  if (state == execution_state::awaiting_input && input.size() > 0)
  {
    state = execution_state::ready;
  }

  while (state == execution_state::ready)
  {
    state = single_step();
  }

  return state;
}

intcode_machine::arg_mode intcode_machine::get_argmode(int64_t opcode, int64_t arg)
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

// Determine the position of the argument 'arg' based on current instruction.
// If opmode of instruction is invalid, sets machine state to execution_state::error.
uint64_t intcode_machine::arg_position(int64_t arg)
{
  int64_t opcode = program[ip];
  arg_mode mode = get_argmode(opcode, arg);
  uint64_t pos = (uint64_t)-1;
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
      pos = (uint64_t)-1;
    }
  }
  if (pos == (uint64_t)-1)
  {
    state = execution_state::invalid_argmode;
  }
  else if (pos + 1 > program.size())
  {
    program.resize(pos + 1, 0);
  }
  return pos;
}

intcode_program read_intcode_program(const char * s)
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

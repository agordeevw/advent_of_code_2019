#pragma once
#include <cstdint>
#include <queue>
#include <utility>
#include <vector>

using intcode_program = std::vector<int64_t>;
intcode_program read_intcode_program(const char* s);

class intcode_machine
{
public:
  enum class execution_state
  {
    ready = 0,
    awaiting_input = 1,
    halted = -1,
    instr_ptr_out_of_bounds = -2,
    invalid_op = -3,
    invalid_argmode = -4,
    unknown_error = -999
  };

  static bool is_valid_state(execution_state state)
  {
    return (int)state >= 0;
  }

  intcode_machine(intcode_program const& program);

  void modify_program(uint64_t address, int64_t value);
  void push_input(int64_t value);

  // If pair.first == true, pair.second contains output value.
  // If pair.first == false, no output was produced.
  std::pair<bool, int64_t> pop_output();

  execution_state single_step();

  // Runs single_step() until machine halts or asks for input when none is provided.
  execution_state run();

  bool has_output() const
  {
    return output.size() > 0;
  }

  execution_state get_state() const
  {
    return state;
  }

private:
  enum class arg_mode
  {
    position = 0,
    immediate = 1,
    relative = 2,
    invalid = -1
  };

  static arg_mode get_argmode(int64_t opcode, int64_t arg);

  uint64_t arg_position(int64_t arg);

  intcode_program program;
  std::queue<int64_t> input;
  std::queue<int64_t> output;
  execution_state state = execution_state::ready;
  uint64_t ip = 0;
  int64_t relative_base = 0;
};
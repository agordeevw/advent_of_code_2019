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
  friend class intcode_machine_inspector;

  enum class execution_state : int
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
  intcode_machine(intcode_machine const& other) = default;
  intcode_machine(intcode_machine&& other) = default;
  intcode_machine& operator=(intcode_machine const& other) = default;
  intcode_machine& operator=(intcode_machine&& other) = default;

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

class intcode_machine_inspector
{
public:
  intcode_machine_inspector(intcode_machine& machine) : machine(machine)
  {
  };

  intcode_program const& program() const
  {
    return machine.program;
  }

  uint64_t ip() const
  {
    return machine.ip;
  }

  uint64_t relative_base() const
  {
    return machine.relative_base;
  }

  void set_ip(uint64_t value)
  {
    machine.ip = value;
  }

  void set_relative_base(uint64_t value)
  {
    machine.relative_base = value;
  }

  void set_execution_state(intcode_machine::execution_state state)
  {
    machine.state = state;
  }

private:
  intcode_machine& machine;
};
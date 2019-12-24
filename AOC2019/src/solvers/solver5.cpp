#include <algorithm>
#include <vector>

#include "solver.hpp"

constexpr int DAY = 5;

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

struct execution_state
{
  std::vector<int64_t> program;
  std::vector<int64_t> input_queue;
  std::vector<int64_t> output_stack;
};

static execution_state execute_program_1(execution_state const& input_state)
{
  execution_state execution_state = input_state;
  uint64_t ip = 0;
  while (ip < execution_state.program.size())
  {
    int64_t opcode = execution_state.program[ip];
    int64_t immediate_a = (opcode / 100) % 10;
    int64_t immediate_b = (opcode / 1000) % 10;
    int64_t immediate_c = (opcode / 10000) % 10;
    switch (opcode % 100)
    {
      case 1:
      {
        int64_t pos_a = immediate_a ? (ip + 1) : execution_state.program[ip + 1];
        int64_t pos_b = immediate_b ? (ip + 2) : execution_state.program[ip + 2];
        int64_t pos_c = immediate_c ? (ip + 3) : execution_state.program[ip + 3];
        execution_state.program[pos_c] = execution_state.program[pos_a] + execution_state.program[pos_b];
        ip += 4;
      }
      break;
      case 2:
      {
        int64_t pos_a = immediate_a ? (ip + 1) : execution_state.program[ip + 1];
        int64_t pos_b = immediate_b ? (ip + 2) : execution_state.program[ip + 2];
        int64_t pos_c = immediate_c ? (ip + 3) : execution_state.program[ip + 3];
        execution_state.program[pos_c] = execution_state.program[pos_a] * execution_state.program[pos_b];
        ip += 4;
      }
      break;
      case 3:
      {
        int64_t pos = execution_state.program[ip + 1];
        int64_t value = execution_state.input_queue.back();
        execution_state.input_queue.pop_back();
        execution_state.program[pos] = value;
        ip += 2;
      }
      break;
      case 4:
      {
        int64_t pos = execution_state.program[ip + 1];
        int64_t value = execution_state.program[pos];
        execution_state.output_stack.push_back(value);
        ip += 2;
      }
      break;
      case 99:
      {
        return execution_state;
      }
    }
  }
  return execution_state;
}

static execution_state execute_program_2(execution_state const& input_state)
{
  execution_state execution_state = input_state;
  uint64_t ip = 0;
  while (ip < execution_state.program.size())
  {
    int64_t opcode = execution_state.program[ip];
    int64_t immediate_a = (opcode / 100) % 10;
    int64_t immediate_b = (opcode / 1000) % 10;
    int64_t immediate_c = (opcode / 10000) % 10;
    switch (opcode % 100)
    {
      // Add
      case 1:
      {
        int64_t pos_a = immediate_a ? (ip + 1) : execution_state.program[ip + 1];
        int64_t pos_b = immediate_b ? (ip + 2) : execution_state.program[ip + 2];
        int64_t pos_c = immediate_c ? (ip + 3) : execution_state.program[ip + 3];
        execution_state.program[pos_c] = execution_state.program[pos_a] + execution_state.program[pos_b];
        ip += 4;
      }
      break;
      // Multiply
      case 2:
      {
        int64_t pos_a = immediate_a ? (ip + 1) : execution_state.program[ip + 1];
        int64_t pos_b = immediate_b ? (ip + 2) : execution_state.program[ip + 2];
        int64_t pos_c = immediate_c ? (ip + 3) : execution_state.program[ip + 3];
        execution_state.program[pos_c] = execution_state.program[pos_a] * execution_state.program[pos_b];
        ip += 4;
      }
      break;
      // Read input
      case 3:
      {
        int64_t pos = execution_state.program[ip + 1];
        int64_t value;
        {
          value = execution_state.input_queue.back();
          execution_state.input_queue.pop_back();
        }
        execution_state.program[pos] = value;
        ip += 2;
      }
      break;
      // Write output
      case 4:
      {
        int64_t pos = execution_state.program[ip + 1];
        int64_t value = execution_state.program[pos];
        execution_state.output_stack.push_back(value);
        ip += 2;
      }
      break;
      // Jump if true
      case 5:
      {
        int64_t pos_a = immediate_a ? (ip + 1) : execution_state.program[ip + 1];
        int64_t pos_b = immediate_b ? (ip + 2) : execution_state.program[ip + 2];
        ip = (execution_state.program[pos_a] != 0) ? execution_state.program[pos_b] : ip + 3;
      }
      break;
      // Jump if false
      case 6:
      {
        int64_t pos_a = immediate_a ? (ip + 1) : execution_state.program[ip + 1];
        int64_t pos_b = immediate_b ? (ip + 2) : execution_state.program[ip + 2];
        ip = (execution_state.program[pos_a] == 0) ? execution_state.program[pos_b] : ip + 3;
      }
      break;
      // Less than
      case 7:
      {
        int64_t pos_a = immediate_a ? (ip + 1) : execution_state.program[ip + 1];
        int64_t pos_b = immediate_b ? (ip + 2) : execution_state.program[ip + 2];
        int64_t pos_c = immediate_c ? (ip + 3) : execution_state.program[ip + 3];
        execution_state.program[pos_c] = execution_state.program[pos_a] < execution_state.program[pos_b] ? 1 : 0;
        ip += 4;
      }
      break;
      // Equals
      case 8:
      {
        int64_t pos_a = immediate_a ? (ip + 1) : execution_state.program[ip + 1];
        int64_t pos_b = immediate_b ? (ip + 2) : execution_state.program[ip + 2];
        int64_t pos_c = immediate_c ? (ip + 3) : execution_state.program[ip + 3];
        execution_state.program[pos_c] = execution_state.program[pos_a] == execution_state.program[pos_b] ? 1 : 0;
        ip += 4;
      }
      break;
      case 99:
      {
        return execution_state;
      }
    }
  }
  return execution_state;
}

}
void solver<DAY, 1>::solve(const char* input, char* output)
{
  std::vector<int64_t> program = read_input(input);
  execution_state initial_state;
  initial_state.program = program;
  initial_state.input_queue = { 1 };
  initial_state.output_stack = {};
  execution_state new_state = execute_program_1(initial_state);
  sprintf(output, "%lld", new_state.output_stack.back());
}

void solver<DAY, 2>::solve(const char* input, char* output)
{
  std::vector<int64_t> program = read_input(input);
  execution_state initial_state;
  initial_state.program = program;
  initial_state.input_queue = { 5 };
  initial_state.output_stack = {};
  execution_state new_state = execute_program_2(initial_state);
  sprintf(output, "%lld", new_state.output_stack.back());
}

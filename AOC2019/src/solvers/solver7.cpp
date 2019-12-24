#include <algorithm>
#include <vector>
#include <queue>

#include "solver.hpp"

constexpr int DAY = 7;
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
  std::vector<int64_t> input_stack;
  std::vector<int64_t> output_stack;
};

static execution_state execute_program(execution_state const& input_state)
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
          value = execution_state.input_stack.back();
          execution_state.input_stack.pop_back();
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

static int64_t run_amplifier_sequence(std::vector<int64_t> const& program, int64_t sequence[5])
{
  execution_state execution_state;
  execution_state.program = program;
  execution_state.input_stack = { 0, sequence[0] };
  int64_t last_amplifier_output = 0;
  last_amplifier_output = execute_program(execution_state).output_stack.back();
  for (int64_t i = 1; i < 5; i++)
  {
    execution_state.input_stack = { last_amplifier_output, sequence[i] };
    last_amplifier_output = execute_program(execution_state).output_stack.back();
  }
  return last_amplifier_output;
}

static int64_t find_best_sequence(std::vector<int64_t> const& program)
{
  int64_t best_output = 0;
  int64_t sequence[5] = { 0, 1, 2, 3, 4 };
  do
  {
    int64_t output = run_amplifier_sequence(program, sequence);
    best_output = output > best_output ? output : best_output;
  } while (std::next_permutation(std::begin(sequence), std::end(sequence)));
  return best_output;
}

// Part 2

struct execution_state_1
{
  bool is_valid() const
  {
    return program.size() > 0 && ip < program.size();
  }

  void halt()
  {
    ip = program.size();
  }

  std::vector<int64_t> program;
  std::queue<int64_t> input_queue;
  std::vector<int64_t> output_stack;
  uint64_t ip = 0;
};

static execution_state_1 execute_program_1(execution_state_1 const& input_state)
{
  execution_state_1 execution_state = input_state;
  uint64_t ip = execution_state.ip;
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
        if (execution_state.input_queue.size() == 0)
        {
          execution_state.ip = ip;
          return execution_state;
        }

        int64_t pos = execution_state.program[ip + 1];
        int64_t value;
        {
          value = execution_state.input_queue.back();
          execution_state.input_queue.pop();
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
      // Halt
      case 99:
      {
        execution_state.halt();
        return execution_state;
      }
    }
  }
  execution_state.ip = ip;
  return execution_state;
}

static int64_t run_amplifier_sequence_2(std::vector<int64_t> const& program, int64_t sequence[5])
{
  execution_state_1 amplifier_states[5];
  for (int i = 0; i < 5; i++)
  {
    amplifier_states[i].program = program;
    amplifier_states[i].input_queue.push(sequence[i]);
    // First run to initialize amplifiers
    execution_state_1 new_state = execute_program_1(amplifier_states[i]);
    amplifier_states[i] = new_state;
  }

  amplifier_states[0].input_queue.push(0);
  int current_amplifier = 0;
  while (true)
  {
    int next_amplifier = (current_amplifier + 1) % 5;
    execution_state_1 new_state = execute_program_1(amplifier_states[current_amplifier]);
    execution_state_1& current_amplifier_state = amplifier_states[current_amplifier];
    execution_state_1& next_amplifier_state = amplifier_states[next_amplifier];

    current_amplifier_state = new_state;
    if (current_amplifier == 4 && !current_amplifier_state.is_valid())
    {
      return current_amplifier_state.output_stack.back();
    }
    int64_t next_amplifier_input = current_amplifier_state.output_stack.back();
    current_amplifier_state.output_stack.pop_back();
    next_amplifier_state.input_queue.push(next_amplifier_input);
    current_amplifier = next_amplifier;
  }

  // unreachable
  return 0;
}

static int64_t find_best_sequence_2(std::vector<int64_t> const& program)
{
  int64_t best_output = 0;
  int64_t sequence[5] = { 5, 6, 7, 8, 9 };
  do
  {
    int64_t output = run_amplifier_sequence_2(program, sequence);
    best_output = output > best_output ? output : best_output;
  } while (std::next_permutation(std::begin(sequence), std::end(sequence)));
  return best_output;
}

}
void solver<DAY, 1>::solve(const char* input, char* output)
{
  std::vector<int64_t> program = read_input(input);
  int64_t amplifier_output = find_best_sequence(program);
  sprintf(output, "%lld", amplifier_output);
}

void solver<DAY, 2>::solve(const char* input, char* output)
{
  std::vector<int64_t> program = read_input(input);
  int64_t amplifier_output = find_best_sequence_2(program);
  sprintf(output, "%lld", amplifier_output);
}

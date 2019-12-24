#include <algorithm>
#include <cassert>
#include <vector>

#include "solver.hpp"
#include "common/vec2.hpp"
#include "common/intcode_machine.hpp"

constexpr int DAY = 21;

namespace
{
struct drone_program
{
  drone_program& op_not(char l, char r)
  {
    buffer_view += sprintf(buffer_view, "NOT %c %c\n", l, r);
    return *this;
  }

  drone_program& op_and(char l, char r)
  {
    buffer_view += sprintf(buffer_view, "AND %c %c\n", l, r);
    return *this;
  }

  drone_program& op_or(char l, char r)
  {
    buffer_view += sprintf(buffer_view, "OR %c %c\n", l, r);
    return *this;
  }

  void op_run()
  {
    buffer_view += sprintf(buffer_view, "RUN\n");
  }

  const char* get() const
  {
    return buffer;
  }

  char buffer[1024] = { 0 };
  char* buffer_view = buffer;
};


enum registers : char
{
  A = 'A',
  B,
  C,
  D,
  E,
  F,
  G,
  H,
  I,
  J = 'J',
  T = 'T'
};
// Put solution helpers here
} // namespace

void solver<DAY, 1>::solve(const char* input, char* output)
{
  auto program = read_intcode_program(input);
  const char* drone_programs[]{
    "NOT A J\nWALK\n",
    "NOT C T\nAND D T\nAND B T\nNOT A J\nOR T J\nWALK\n",
    "NOT B T\nAND D T\nAND C T\nNOT A J\nOR T J\nWALK\n",
    "NOT B T\nNOT C J\nAND J T\nAND D T\nNOT A J\nOR T J\nWALK\n",
    "NOT C J\nAND B J\nNOT B T\nAND C T\nOR J T\nAND D T\nNOT A J\nOR T J\nWALK\n",
    "NOT C T\nAND D T\nNOT A J\nOR T J\nWALK\n",
    "NOT B T\nAND D T\nNOT A J\nOR T J\nWALK\n",
    "NOT C J\nAND B J\nNOT B T\nAND J T\nAND D T\nNOT A J\nOR T J\nWALK\n"
  };
  for (auto drone_program : drone_programs)
  {
    intcode_machine machine{ program };
    const char* s = drone_program;
    while (*s)
    {
      machine.push_input((int64_t)(*s));
      s++;
    }
    machine.run();
    output += sprintf(output, "Program:\n%s\n", drone_program);
    while (true)
    {
      auto out = machine.pop_output();
      if (out.first == false)
      {
        break;
      }
      if (out.second < 256)
      {
        output += sprintf(output, "%c", (char)(out.second));
      }
      else
      {
        output += sprintf(output, "%lld\n", out.second);
      }
    }
  }
}

void solver<DAY, 2>::solve(const char* input, char* output)
{
  // !A | (D & ( (B & !C) | (!B & C) | (B & !C & E & !F)))
  // ##.#..###
  // ##.##.##.
  // #.##...##

  intcode_machine machine{ read_intcode_program(input) };
  drone_program drone_pr;

  drone_pr
    .op_or(A, J)
    .op_and(B, J)
    .op_and(C, J)
    .op_not(J, J)
    .op_and(D, J)
    .op_or(E, T)
    .op_or(H, T)
    .op_and(T, J)
    .op_run();
  const char* s = drone_pr.get();
  while (*s)
  {
    machine.push_input((int64_t)(*s));
    s++;
  }
  machine.run();
  while (true)
  {
    auto out = machine.pop_output();
    if (out.first == false)
    {
      break;
    }
    if (out.second < 256)
    {
      output += sprintf(output, "%c", (char)(out.second));
    }
    else
    {
      output += sprintf(output, "%lld\n", out.second);
    }
  }
}

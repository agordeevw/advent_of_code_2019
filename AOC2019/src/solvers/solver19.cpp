#include <algorithm>
#include <cassert>
#include <vector>

#include "solver.hpp"
#include "common/vec2.hpp"
#include "common/intcode_machine.hpp"

constexpr int DAY = 19;

void solver<DAY, 1>::solve(const char* input, char* output)
{
  intcode_program program = read_intcode_program(input);
  int64_t num_points = 0;
  for (int y = 0; y < 50; y++)
  {
    for (int x = 0; x < 50; x++)
    {
      intcode_machine machine(program);
      machine.push_input(x);
      machine.push_input(y);
      machine.run();
      num_points += machine.pop_output().second;
    }
  }
  output += sprintf(output, "%lld\n", num_points);
}

void solver<DAY, 2>::solve(const char* input, char* output)
{
  intcode_program program = read_intcode_program(input);
  for (int y = 0; y < 100; y++)
  {
    for (int x = 0; x < 100; x++)
    {
      intcode_machine machine(program);
      machine.push_input(x);
      machine.push_input(y);
      machine.run();
      output += sprintf(output, "%c", machine.pop_output().second ? '#' : '.');
    }
    output += sprintf(output, "\n");
  }

  auto is_attracting = [&program](vec2 p) -> bool
  {
    intcode_machine machine(program);
    machine.push_input(p.x);
    machine.push_input(p.y);
    machine.run();
    return machine.pop_output().second != 0;
  };

  std::vector<vec2> interval_for_y(2000);
  interval_for_y[7] = {3, 3};
  const int start_y = 800;
  for (int y = start_y; y < interval_for_y.size(); y++)
  {
    int l = interval_for_y[y-1].x;
    while (is_attracting({ l, y }) == false)
    {
      l++;
    }

    int r = l;
    while (is_attracting({ r, y }) == true)
    {
      r++;
    }
    interval_for_y[y] = { l, r - 1 };
  }

  vec2 target = {INT_MAX, 0};
  for (int y = start_y; y < interval_for_y.size() - 100; y++)
  {
    vec2 i = interval_for_y[y];
    vec2 li = interval_for_y[y + 99];
    if (li.x + 99 == i.y)
    {
      target = {li.x, y};
      break;
    }
  }

  if (target.x != INT_MAX)
  {
  output += sprintf(output, "%d\n", 10000 * target.x + target.y);
  }
  else
  {
    output += sprintf(output, "failure\n");
  }
}

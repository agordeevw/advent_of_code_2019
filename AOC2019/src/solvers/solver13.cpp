#include <algorithm>
#include <cassert>
#include <unordered_map>
#include <vector>

#include "common/intcode_machine.hpp"
#include "solver.hpp"

constexpr int DAY = 13;

void solver<DAY, 1>::solve(const char* input, char* output)
{
  intcode_program program = read_intcode_program(input);
  intcode_machine machine(program);
  machine.run();
  assert(machine.get_state() == intcode_machine::execution_state::halted);
  int64_t num_blocks = 0;
  int64_t ball_x = 0;
  int64_t paddle_x = 0;
  while (machine.has_output())
  {
    int64_t x = machine.pop_output().second;
    assert(machine.has_output());
    int64_t y = machine.pop_output().second;
    (void)y;
    assert(machine.has_output());
    int64_t tileid = machine.pop_output().second;
    if (tileid == 2)
    {
      num_blocks++;
    }
    else if (tileid == 3)
    {
      paddle_x = x;
    }
    else if (tileid == 4)
    {
      ball_x = x;
    }
  }
  sprintf(output, "%lld", num_blocks);
}

void solver<DAY, 2>::solve(const char* input, char* output)
{
  intcode_program program = read_intcode_program(input);

  int64_t num_blocks = 0;
  int64_t ball_x = 0;
  int64_t paddle_x = 0;
  const int64_t min_x = 0;
  const int64_t max_x = 37;
  const int64_t min_y = 0;
  const int64_t max_y = 20;
  std::vector<char> frame(38 * 21);
  {
    intcode_machine machine(program);
    machine.run();
    assert(machine.get_state() == intcode_machine::execution_state::halted);
    while (machine.has_output())
    {
      int64_t x, y, tileid;
      x = machine.pop_output().second;
      y = machine.pop_output().second;
      tileid = machine.pop_output().second;

      if (x >= min_x && x <= max_x && y >= min_y && y <= max_y)
      {
        frame[x + 38 * y] = tileid == 0 ? ' ' : tileid == 1 ? 'X' : tileid == 2 ? '-' : tileid == 3 ? '_' : 'o';
      }
    }
  }

  intcode_machine machine(program);
  machine.modify_program(0, 2);
  int64_t score = 0;
  do
  {
    for (int y = min_y; y <= max_y; y++)
    {
      for (int x = min_x; x <= max_x; x++)
      {
        if (frame[x + 38 * y] == 'o')
        {
          ball_x = x;
        }
        if (frame[x + 38 * y] == '_')
        {
          paddle_x = x;
        }
      }
    }

    int joystick = 0;
    if (paddle_x < ball_x) joystick = 1;
    if (paddle_x > ball_x) joystick = -1;
    machine.push_input(joystick);
    machine.run();

    while (machine.has_output())
    {
      int64_t x, y, tileid;
      x = machine.pop_output().second;
      y = machine.pop_output().second;
      tileid = machine.pop_output().second;
      if (x >= min_x && x <= max_x && y >= min_y && y <= max_y)
      {
        frame[x + 38 * y] = tileid == 0 ? ' ' : tileid == 1 ? 'X' : tileid == 2 ? '-' : tileid == 3 ? '_' : 'o';
      }
      if (x == -1 && y == 0)
      {
        score = tileid;
      }
    }

    num_blocks = 0;
    for (int y = min_y; y <= max_y; y++)
    {
      for (int x = min_x; x <= max_x; x++)
      {
        if (frame[x + 38 * y] == '-')
        {
          num_blocks++;
        }
      }
    }
  } while (num_blocks > 0);

  sprintf(output, "%lld", score);
}

#include <algorithm>
#include <cassert>
#include <vector>

#include "solver.hpp"
#include "common/vec2.hpp"
#include "common/intcode_machine.hpp"

constexpr int DAY = 17;

void solver<DAY, 1>::solve(const char* input, char* output)
{
  intcode_machine machine(read_intcode_program(input));
  machine.run();
  std::vector<char> map;
  while (true)
  {
    auto c = machine.pop_output();
    if (c.first == false)
    {
      break;
    }
    map.push_back((char)c.second);
  }
  map.push_back(0);
  printf("%s", map.data());
  int map_width = 0;
  int map_height = 0;
  for (auto c : map)
  {
    if (c == '\n')
    {
      break;
    }
    map_width++;
  }
  map_height = (int)(map.size() - 1) / (map_width + 1);
  output += sprintf(output, "%s", map.data());

  auto map_at = [&map, map_height, map_width](int x, int y) -> char
  {
    return map[x + (map_width + 1) * y];
  };

  int sum = 0;
  for (int y = 1; y < map_height - 1; y++)
  {
    for (int x = 1; x < map_width - 1; x++)
    {
      bool c = map_at(x, y) == '#';
      bool r = map_at(x + 1, y) == '#';
      bool l = map_at(x - 1, y) == '#';
      bool b = map_at(x, y + 1) == '#';
      bool t = map_at(x, y - 1) == '#';
      if (c && r && l && b && t)
      {
        sum += x * y;
      }
    }
  }
  output += sprintf(output, "\n%d\n", sum);
}

void solver<DAY, 2>::solve(const char* input, char* output)
{
  std::vector<char> map;
  int map_width = 0;
  int map_height = 0;

  intcode_program program = read_intcode_program(input);
  {
    intcode_machine machine(program);
    machine.run();
    while (true)
    {
      auto c = machine.pop_output();
      if (c.first == false)
      {
        break;
      }
      map.push_back((char)c.second);
    }
    map.push_back(0);
    printf("%s", map.data());
    for (auto c : map)
    {
      if (c == '\n')
      {
        break;
      }
      map_width++;
    }
    map_height = (int)(map.size() - 1) / (map_width + 1);
    output += sprintf(output, "%s", map.data());
  }

  auto map_at = [&map, &map_height, &map_width](int x, int y) -> char
  {
    return map[x + (map_width + 1) * y];
  };

  auto map_in_bounds = [&map_height, &map_width](int x, int y) -> bool
  {
    return x >= 0 && x < map_width && y >= 0 && y < map_height;
  };

  vec2 pos = { INT_MAX, INT_MAX };
  int dir = 0; // 0 - up, 1 - right, 2 - down, 3 - left
  {
    {
      for (int y = 0; y < map_height; y++)
      {
        for (int x = 0; x < map_width; x++)
        {
          if (map_at(x, y) == '^')
          {
            pos = { x, y };
            goto pos_found;
          }
        }
      }
    }
  }
pos_found:

  std::vector<int64_t> path_program;
  while (true)
  {
    vec2 at_right;
    bool can_go_right;
    {
      at_right = pos;
      switch (dir)
      {
        case 0: at_right.x++; break;
        case 1: at_right.y++; break;
        case 2: at_right.x--; break;
        case 3: at_right.y--; break;
      }
      can_go_right = map_in_bounds(at_right.x, at_right.y) && map_at(at_right.x, at_right.y) == '#';
    }
    vec2 at_left = pos;
    bool can_go_left;
    {
      at_left = pos;
      {
        switch (dir)
        {
          case 0: at_left.x--; break;
          case 1: at_left.y--; break;
          case 2: at_left.x++; break;
          case 3: at_left.y++; break;
        }
      }
      can_go_left = map_in_bounds(at_left.x, at_left.y) && map_at(at_left.x, at_left.y) == '#';
    }
    if (can_go_left == false && can_go_right == false)
    {
      break;
    }
    int walk_dist = 0;
    if (can_go_left)
    {
      path_program.push_back('L');
      dir = (dir == 0) ? 3 : ((dir - 1) % 4);
    }
    else if (can_go_right)
    {
      path_program.push_back('R');
      dir = (dir + 1) % 4;
    }
    while (true)
    {
      vec2 at_forward;
      bool can_go_forward;
      {
        at_forward = pos;
        switch (dir)
        {
          case 0: at_forward.y--; break;
          case 1: at_forward.x++; break;
          case 2: at_forward.y++; break;
          case 3: at_forward.x--; break;
        }
        can_go_forward = map_in_bounds(at_forward.x, at_forward.y) && map_at(at_forward.x, at_forward.y) == '#';
      }
      if (can_go_forward == false)
      {
        path_program.push_back(walk_dist);
        break;
      }
      else
      {
        walk_dist++;
        pos = at_forward;
      }
    }
  }

  for (auto c : path_program)
  {
    if (c == 'R' || c == 'L')
    {
      output += sprintf(output, "%c,", (char)c);
    }
    else
    {
      output += sprintf(output, "%lld,", c);
    }
  }
  output += sprintf(output, "\n");

  char bot_input[] = "A,B,B,C,C,A,B,B,C,A\n"
    "R,4,R,12,R,10,L,12\n"
    "L,12,R,4,R,12\n"
    "L,12,L,8,R,10\n"
    "n\n";

  intcode_machine machine(program);
  machine.modify_program(0, 2);
  for (char c : bot_input)
  {
    if (c == 0)
    {
      break;
    }
    machine.push_input((int64_t)c);
  }
  machine.run();

  // map before
  for (int y = 0; y < map_height; y++)
  {
    for (int x = 0; x < map_width; x++)
    {
      machine.pop_output();
      machine.pop_output();
    }
  }

  // Wrong, need to fetch last value from output
  auto out = machine.pop_output();
  assert(out.first);
  output += sprintf(output, "Dust: %lld\n", out.second);
}

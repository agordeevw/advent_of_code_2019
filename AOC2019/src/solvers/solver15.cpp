#include <algorithm>
#include <cassert>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "common/intcode_machine.hpp"
#include "solver.hpp"
#include "common/a_star.hpp"
#include "common/vec2.hpp"

constexpr int DAY = 15;

namespace
{

enum dir : int
{
  north = 1,
  south = 2,
  west = 3,
  east = 4
};

using map_type = std::unordered_map<vec2, int, vec2_hasher>;

std::vector<dir> find_path_with_templated_a_star(map_type const& map, vec2 start, vec2 finish)
{
  auto list_adjacent = [&map, finish](vec2 p) -> std::vector<vec2>
  {
    vec2 neighbours[] = {
      vec2{p.x + 1, p.y},
      vec2{p.x - 1, p.y},
      vec2{p.x, p.y - 1},
      vec2{p.x, p.y + 1}
    };

    std::vector<vec2> ret;
    for (vec2 const& next : neighbours)
    {
      if (map.find(next) != map.end())
      {
        int map_value = map.at(next);
        if (map_value == 1 || map_value == 2 || (map_value == -1 && next == finish))
        {
          ret.push_back(next);
        }
      }
    }

    return ret;
  };

  auto transition_cost = [&map](vec2, vec2) -> int64_t
  {
    return 1;
  };

  auto heuristic = [&map](vec2 start, vec2 finish) -> int64_t
  {
    return std::abs(finish.x - start.x) + std::abs(finish.y - start.y);
  };

  std::vector<vec2> path = a_star<vec2, vec2_hasher, int64_t, decltype(list_adjacent), decltype(transition_cost), decltype(heuristic)>(start, finish, list_adjacent, transition_cost, heuristic);

  vec2 cur_p = path.back();
  path.pop_back();
  std::vector<dir> dirs_path;
  while (path.size() > 0)
  {
    vec2 next_p = path.back();
    path.pop_back();
    if (cur_p.x + 1 == next_p.x)
    {
      dirs_path.push_back(dir::east);
    }
    else if (cur_p.x - 1 == next_p.x)
    {
      dirs_path.push_back(dir::west);
    }
    else if (cur_p.y + 1 == next_p.y)
    {
      dirs_path.push_back(dir::north);
    }
    else if (cur_p.y - 1 == next_p.y)
    {
      dirs_path.push_back(dir::south);
    }
    cur_p = next_p;
  }

  return dirs_path;
}

} // namespace

void solver<DAY, 1>::solve(const char* input, char* output)
{
  intcode_program program = read_intcode_program(input);
  intcode_machine machine(program);
  vec2 droid_pos = { 0, 0 };

  map_type map;
  vec2 global_target{ INT_MAX, INT_MAX };
  map[{0, 0}] = 1;
  map[{1, 0}] = -1;
  map[{-1, 0}] = -1;
  map[{0, 1}] = -1;
  map[{0, -1}] = -1;

  std::vector<vec2> unknown{ {1, 0}, {0, 1}, {-1, 0}, {0, -1} };
  while (unknown.size() > 0)
  {
    // Get point to inspect
    vec2 unknown_target = unknown.back();
    unknown.pop_back();
    map[unknown_target] = -1;

    // Obtain path to put the droid to the point
    std::vector<dir> path = find_path_with_templated_a_star(map, droid_pos, unknown_target);
    for (dir d : path)
    {
      machine.push_input((int64_t)d);
      machine.run();
      int64_t status = machine.pop_output().second;
      vec2 target_pos = droid_pos;
      switch (d)
      {
        case dir::north: target_pos.y++; break;
        case dir::south: target_pos.y--; break;
        case dir::west: target_pos.x--; break;
        case dir::east: target_pos.x++; break;
      }
      map[target_pos] = (int)status;
      if (status != 0)
      {
        droid_pos = target_pos;
      }
      if (status == 2)
      {
        global_target = droid_pos;
      }
    }
    if (droid_pos == unknown_target)
    {
      for (int i = 1; i <= 4; i++)
      {
        vec2 new_unknown = droid_pos;
        switch ((dir)i)
        {
          case dir::north: new_unknown.y++; break;
          case dir::south: new_unknown.y--; break;
          case dir::west: new_unknown.x--; break;
          case dir::east: new_unknown.x++; break;
        }
        if (map.find(new_unknown) == map.end())
        {
          map[new_unknown] = -1;
          unknown.push_back(new_unknown);
        }
      }
    }
  }

  vec2 min_map = { INT_MAX, INT_MAX };
  vec2 max_map = { INT_MIN, INT_MIN };
  for (auto const& p : map)
  {
    min_map.x = std::min(min_map.x, p.first.x);
    min_map.y = std::min(min_map.y, p.first.y);
    max_map.x = std::max(max_map.x, p.first.x);
    max_map.y = std::max(max_map.y, p.first.y);
  }

  for (int y = max_map.y; y >= min_map.y; y--)
  {
    for (int x = min_map.x; x <= max_map.x; x++)
    {
      if (map.find({ x, y }) != map.end())
      {
        if (vec2{ x, y } == droid_pos)
        {
          printf("@");
        }
        else if (vec2{ x, y } == global_target)
        {
          printf("$");
        }
        else
        {
          switch (map.at({ x, y }))
          {
            case 0: printf("#"); break;
            case 1: printf(" "); break;
            case -1: printf("?"); break;
            default: printf("-"); break;
          }
        }
      }
      else
      {
        printf("-");
      }
    }
    printf("\n");
  }
  printf("Map size: (%d, %d) - (%d, %d)\n", min_map.x, min_map.y, max_map.x, max_map.y);
  printf("Droid pos: (%d, %d)\n", droid_pos.x, droid_pos.y);
  printf("\n");

  sprintf(output, "%llu", find_path_with_templated_a_star(map, { 0, 0 }, global_target).size());
  output += strlen(output);

  // Flow from oxygen tank
  auto map_has_free_place = [](map_type const& map) -> bool
  {
    return std::any_of(map.begin(), map.end(), [](std::pair<vec2, int> const& p)
    {
      return p.second == 1;
    });
  };

  int fill_iter = 0;
  while (map_has_free_place(map))
  {
    fill_iter++;
    map_type tmp_map = map;
    for (auto p : map)
    {
      if (p.second == 2)
      {
        for (int i = 1; i <= 4; i++)
        {
          vec2 next = p.first;
          switch ((dir)i)
          {
            case dir::north: next.y++; break;
            case dir::south: next.y--; break;
            case dir::west: next.x--; break;
            case dir::east: next.x++; break;
          }
          if (map.at(next) == 1)
          {
            tmp_map[next] = 2;
          }
        }
      }
    }
    map = tmp_map;
  }
  sprintf(output, "\n%d", fill_iter);
}

void solver<DAY, 2>::solve(const char* input, char* output)
{
  (void)input;
  (void)output;
}

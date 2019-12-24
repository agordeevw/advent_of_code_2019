#include <algorithm>
#include <cassert>
#include <vector>
#include <string>
#include <unordered_set>

#include "solver.hpp"
#include "common/vec2.hpp"
#include "common/a_star.hpp"

constexpr int DAY = 18;

namespace {

enum class map_tile
{
  entrance = '@',
  passage = '.',
  wall = '#',
  key0 = 'a',
  door0 = 'A'
};

struct map_type
{
  bool in_bounds(vec2 p) const
  {
    return p.x >= 0 && p.x < width && p.y >= 0 && p.y < height;
  }

  char at(vec2 p) const
  {
    return data[p.x + (width + 1) * p.y];
  }

  char& at(vec2 p)
  {
    return data[p.x + (width + 1) * p.y];
  }

  std::vector<char> data;
  int width;
  int height;
};

static map_type read_map(const char* s)
{
  map_type ret;
  ret.width = 0;
  ret.height = 0;
  while (*s)
  {
    ret.data.push_back(*s);
    s++;
  }
  for (auto c : ret.data)
  {
    if (c == '\n')
    {
      break;
    }
    ret.width++;
  }
  for (auto c : ret.data)
  {
    if (c == '\n')
    {
      ret.height++;
    }
  }
  return ret;
}

void eliminate_dead_ends(map_type& map)
{
  bool dead_end_found = false;
  do
  {
    dead_end_found = false;
    for (int y = 1; y < map.height - 1; y++)
    {
      for (int x = 1; x < map.width - 1; x++)
      {
        char c = map.at({ x, y });
        char d = map.at({ x, y + 1 });
        char u = map.at({ x, y - 1 });
        char l = map.at({ x - 1, y });
        char r = map.at({ x + 1, y });
        int num_surrounding_walls = 0;
        if (u == '#') num_surrounding_walls++;
        if (d == '#') num_surrounding_walls++;
        if (l == '#') num_surrounding_walls++;
        if (r == '#') num_surrounding_walls++;
        if (c == '.' && num_surrounding_walls == 3)
        {
          dead_end_found = true;
          map.at({ x, y }) = '#';
        }
      }
    }
  } while (dead_end_found == true);
}

bool eliminate_dead_end_doors(map_type& map)
{
  auto is_door = [](char c) -> bool
  {
    return c >= 'A' && c <= 'Z';
  };

  bool door_found = false;
  for (int y = 1; y < map.height - 1; y++)
  {
    for (int x = 1; x < map.width - 1; x++)
    {
      char c = map.at({ x, y });
      char d = map.at({ x, y + 1 });
      char u = map.at({ x, y - 1 });
      char l = map.at({ x - 1, y });
      char r = map.at({ x + 1, y });
      int num_surrounding_walls = 0;
      if (u == '#') num_surrounding_walls++;
      if (d == '#') num_surrounding_walls++;
      if (l == '#') num_surrounding_walls++;
      if (r == '#') num_surrounding_walls++;
      if (is_door(c) && num_surrounding_walls == 3)
      {
        door_found = true;
        map.at({ x, y }) = '#';
      }
    }
  }
  return door_found;
}

void mark_intersections(map_type& map)
{
  for (int y = 1; y < map.height - 1; y++)
  {
    for (int x = 1; x < map.width - 1; x++)
    {
      char c = map.at({ x, y });
      char d = map.at({ x, y + 1 });
      char u = map.at({ x, y - 1 });
      char l = map.at({ x - 1, y });
      char r = map.at({ x + 1, y });
      int num_surrounding_walls = 0;
      if (u == '#') num_surrounding_walls++;
      if (d == '#') num_surrounding_walls++;
      if (l == '#') num_surrounding_walls++;
      if (r == '#') num_surrounding_walls++;
      if (c == '.' && num_surrounding_walls <= 1)
      {
        map.at({ x, y }) = '+';
      }
    }
  }
}

auto find_path_a_star(map_type const& map, vec2 from, vec2 to)
{
  auto list_adjacent = [&map](vec2 p) -> std::vector<vec2>
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
      if (map.at(next) != '#')
      {
        ret.push_back(next);
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
    int64_t dx = std::abs(finish.x - start.x);
    int64_t dy = std::abs(finish.y - start.y);
    return dx + dy;
  };

  return a_star<vec2, vec2_hasher, int64_t, decltype(list_adjacent), decltype(transition_cost), decltype(heuristic)>(from, to, list_adjacent, transition_cost, heuristic);
}

auto find_path(map_type const& map, vec2 from, vec2 to)
{
  auto list_adjacent = [&map](vec2 p) -> std::vector<vec2>
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
      if (map.at(next) != '#')
      {
        ret.push_back(next);
      }
    }

    return ret;
  };

  struct bfs
  {
    vec2 p;
    int d;
  };

  std::unordered_set<vec2, vec2_hasher> visited;
  std::queue<bfs> to_visit;
  std::unordered_map<vec2, vec2, vec2_hasher> came_from;
  to_visit.push({ from, 0 });
  while (to_visit.size() > 0)
  {
    bfs cur = to_visit.front();
    to_visit.pop();
    visited.emplace(cur.p);

    if (cur.p == to)
    {
      break;
    }

    for (vec2 p : list_adjacent(cur.p))
    {
      if (visited.find(p) == visited.end())
      {
        to_visit.push({ p, cur.d + 1 });
        came_from[p] = cur.p;
      }
    }
  }

  std::vector<vec2> ret;
  ret.push_back(to);
  vec2 p = to;
  while (!(p == from))
  {
    p = came_from.at(p);
    ret.push_back(p);
  }

  return ret;
}

} // namespace

uint64_t solve_1(map_type const& map, char* out_str)
{
  vec2 key_pos[26];
  vec2 start_pos = {};
  for (int i = 0; i < 26; i++)
  {
    for (int y = 0; y < map.height; y++)
    {
      for (int x = 0; x < map.width; x++)
      {
        if (map.at({ x, y }) == (char)('a' + i))
        {
          key_pos[i] = { x, y };
        }
        else if (map.at({ x, y }) == '@')
        {
          start_pos = { x, y };
        }
      }
    }
  }

  uint64_t key_to_key_dist[26][26];
  uint64_t start_to_key_dist[26];
  uint32_t req_keys[26][26];
  for (int src = 0; src < 26; src++)
  {
    start_to_key_dist[src] = find_path_a_star(map, key_pos[src], start_pos).size() - 1;
    for (int dst = src; dst < 26; dst++)
    {
      key_to_key_dist[src][dst] = find_path_a_star(map, key_pos[src], key_pos[dst]).size() - 1;
      {
        auto path = find_path(map, key_pos[src], key_pos[dst]);
        auto a_star_path = find_path_a_star(map, key_pos[src], key_pos[dst]);
        if (a_star_path.size() != path.size())
        {
          fprintf(stderr, "a_star failed at (%d, %d)->(%d, %d), cost %llu, actual %llu\n",
                  key_pos[src].x, key_pos[src].y, key_pos[dst].x, key_pos[dst].y,
                  a_star_path.size() - 1, path.size() - 1);
          for (int i = 0; i < path.size(); i++)
          {
            if (!(a_star_path[i] == path[i]))
            {
              fprintf(stderr, "discrepancy at (%d, %d):\n",
                      path[i].x, path[i].y);
              for (int j = 0; j < 8; j++)
              {
                int idx = i - 1 + j;
                fprintf(stderr, "%d: bfs (%d, %d) | astar (%d, %d)\n",
                        j + 1, path[idx].x, path[idx].y, a_star_path[idx].x, a_star_path[idx].y);
              }
              break;
            }
          }
        }
      }
      key_to_key_dist[dst][src] = key_to_key_dist[src][dst];
      req_keys[src][dst] = 0;
      if (src != dst)
      {
        auto path = find_path_a_star(map, key_pos[src], key_pos[dst]);
        for (auto p : path)
        {
          char c = map.at(p);
          if (c >= 'A' && c <= 'Z')
          {
            req_keys[src][dst] |= (1 << (c - 'A'));
          }
        }
        req_keys[dst][src] = req_keys[src][dst];
      }
    }
  }

  struct state
  {
    state() : at_key{ -1 }, key_found_mask{ 0 }
    {
    }

    state(int at_key) : at_key(at_key), key_found_mask{ 0 }
    {
    }

    bool operator==(state const& other) const
    {
      return at_key == other.at_key && key_found_mask == other.key_found_mask;
    }

    inline bool key_found(int i) const
    {
      return key_found_mask & (1 << i);
    }

    bool is_final() const
    {
      return (key_found_mask & 0x03FFFFFF) == 0x03FFFFFF;
    }

    void mark_key(int i)
    {
      key_found_mask |= (1 << i);
      key_seq.push_back((char)('a' + i));
    }

    int at_key;
    uint32_t key_found_mask;
    std::string key_seq;
  };

  struct state_hasher
  {
    uint64_t operator()(state const& s) const
    {
      return s.key_found_mask;
    }
  };

  uint64_t best_steps = UINT64_MAX;
  char best_str[32] = { 0 };
  std::unordered_map<state, uint64_t, state_hasher> map_state_to_steps;
  std::vector<state> to_visit = { {-1} };
  map_state_to_steps[{-1}] = 0;
  while (to_visit.size() > 0)
  {
    state cur_state = to_visit.back();
    to_visit.pop_back();
    uint64_t cur_steps = map_state_to_steps.at(cur_state);

    if (cur_state.is_final())
    {
      if (cur_steps < best_steps)
      {
        best_steps = cur_steps;
        strcpy(best_str, cur_state.key_seq.c_str());
        printf("%llu (%s)\n", best_steps, cur_state.key_seq.c_str());
      }
      continue;
    }

    for (int key_cand = 0; key_cand < 26; key_cand++)
    {
      if (cur_state.key_found(key_cand) == false)
      {
        // ERROR: cur_state.at_key is equal -1 at first iteration
        uint32_t req_keys_mask = req_keys[cur_state.at_key][key_cand];
        if (req_keys_mask == 0 || (cur_state.key_found_mask & req_keys_mask) == req_keys_mask)
        {
          state new_state = cur_state;
          new_state.at_key = key_cand;
          new_state.mark_key(key_cand);
          uint64_t new_steps = cur_steps + (cur_state.at_key == -1 ? start_to_key_dist[key_cand]
                                            : key_to_key_dist[cur_state.at_key][key_cand]);

          if (map_state_to_steps.find(new_state) == map_state_to_steps.end()
              || new_steps < map_state_to_steps.at(new_state))
          {
            map_state_to_steps[new_state] = new_steps;
            to_visit.push_back(new_state);
          }
        }
      }
    }
  }

  strcpy(out_str, best_str);
  return best_steps;
}

void solver<DAY, 1>::solve(const char* input, char* output)
{
  map_type map = read_map(input);

  vec2 key_pos[26];
  vec2 start_pos = {};
  for (int i = 0; i < 26; i++)
  {
    for (int y = 0; y < map.height; y++)
    {
      for (int x = 0; x < map.width; x++)
      {
        if (map.at({ x, y }) == (char)('a' + i))
        {
          key_pos[i] = { x, y };
        }
        else if (map.at({ x, y }) == '@')
        {
          start_pos = { x, y };
        }
      }
    }
  }

  {
    while (true)
    {
      eliminate_dead_ends(map);
      bool eliminated_doors = eliminate_dead_end_doors(map);
      if (eliminated_doors == false)
      {
        break;
      }
    }
    mark_intersections(map);
    map.data.push_back(0);
    output += sprintf(output, "%s\n\n", map.data.data());
  }

  char best_str[64];
  uint64_t best_len = solve_1(map, best_str);
  output += sprintf(output, "%s: %llu\n", best_str, best_len);

  vec2 from = start_pos;
  vec2 to = key_pos[best_str[0] - 'a'];
  output += sprintf(output, "@ -> %c: ", best_str[0]);
  for (int i = 0; i < strlen(best_str); i++)
  {
    auto path = find_path(map, from, to);
    output += sprintf(output, "%llu\n", path.size() - 1);
    if (i == strlen(best_str) - 1)
    {
      break;
    }
    output += sprintf(output, "%c -> %c: ", best_str[i], best_str[i + 1]);
    from = to;
    to = key_pos[best_str[i + 1] - 'a'];
  }
}

uint64_t solve_2(map_type const& map)
{
  vec2 key_pos[26];
  vec2 start_pos[4];
  for (int i = 0; i < 26; i++)
  {
    for (int y = 0; y < map.height; y++)
    {
      for (int x = 0; x < map.width; x++)
      {
        if (map.at({ x, y }) == (char)('a' + i))
        {
          key_pos[i] = { x, y };
        }
      }
    }
  }
  start_pos[0] = { 39, 39 };
  start_pos[1] = { 39, 41 };
  start_pos[2] = { 41, 41 };
  start_pos[3] = { 41, 39 };

  uint64_t key_to_key_dist[26][26];
  uint32_t req_keys_start_to_key[4][26];
  uint64_t start_to_key_dist[4][26];
  uint32_t req_keys_key_to_key[26][26];
  for (int src = 0; src < 26; src++)
  {
    for (int i = 0; i < 4; i++)
    {
      auto path = find_path_a_star(map, start_pos[i], key_pos[src]);
      start_to_key_dist[i][src] = path.size() - 1;
      for (auto p : path)
      {
        char c = map.at(p);
        if (c >= 'A' && c <= 'Z')
        {
          req_keys_start_to_key[i][src] |= (1 << (c - 'A'));
        }
      }
    }
    for (int dst = src; dst < 26; dst++)
    {
      key_to_key_dist[src][dst] = find_path_a_star(map, key_pos[src], key_pos[dst]).size() - 1;
      key_to_key_dist[dst][src] = key_to_key_dist[src][dst];
      req_keys_key_to_key[src][dst] = 0;
      if (src != dst)
      {
        auto path = find_path_a_star(map, key_pos[src], key_pos[dst]);
        for (auto p : path)
        {
          char c = map.at(p);
          if (c >= 'A' && c <= 'Z')
          {
            req_keys_key_to_key[src][dst] |= (1 << (c - 'A'));
          }
        }
        req_keys_key_to_key[dst][src] = req_keys_key_to_key[src][dst];
      }
    }
  }

  struct state
  {
    state() : at_key{ -1, -1, -1, -1 }, key_found_mask{ 0 }
    {
    }

    state(int a, int b, int c, int d) : at_key{ a, b, c, d }, key_found_mask{ 0 } {}

    bool operator==(state const& other) const
    {
      for (int i = 0; i < 4; i++)
      {
        if (at_key[i] != other.at_key[i])
        {

          return false;
        }}
      return key_found_mask == other.key_found_mask;
    }

    inline bool key_found(int i) const
    {
      return key_found_mask & (1 << i);
    }

    bool is_final() const
    {
      return (key_found_mask & 0x03FFFFFF) == 0x03FFFFFF;
    }

    void mark_key(int i)
    {
      key_found_mask |= (1 << i);
    }

    int at_key[4];
    uint32_t key_found_mask;
  };

  struct state_hasher
  {
    uint64_t operator()(state const& s) const
    {
      return s.key_found_mask;
    }
  };

  uint64_t best_steps = UINT64_MAX;
  std::unordered_map<state, uint64_t, state_hasher> map_state_to_steps;
  std::vector<state> to_visit = { {-1, -1, -1, -1} };
  map_state_to_steps[{-1, -1, -1, -1}] = 0;
  while (to_visit.size() > 0)
  {
    state cur_state = to_visit.back();
    to_visit.pop_back();
    uint64_t cur_steps = map_state_to_steps.at(cur_state);

    if (cur_state.is_final())
    {
      if (cur_steps < best_steps)
      {
        best_steps = cur_steps;
        printf("%llu\n", best_steps);
      }
      continue;
    }

    for (int i = 0; i < 4; i++)
    {
      for (int key_cand = 0; key_cand < 26; key_cand++)
      {
        if (cur_state.key_found(key_cand) == false)
        {
          uint32_t req_keys_mask = cur_state.at_key[i] == -1 ?
            req_keys_start_to_key[i][key_cand] : req_keys_key_to_key[cur_state.at_key[i]][key_cand];
          uint64_t dist = cur_state.at_key[i] == -1 ? start_to_key_dist[i][key_cand]
                                              : key_to_key_dist[cur_state.at_key[i]][key_cand];
          if (dist != 0 && (req_keys_mask == 0 || (cur_state.key_found_mask & req_keys_mask) == req_keys_mask))
          {
            state new_state = cur_state;
            new_state.at_key[i] = key_cand;
            new_state.mark_key(key_cand);
            uint64_t new_steps = cur_steps + dist;

            if (map_state_to_steps.find(new_state) == map_state_to_steps.end()
                || new_steps < map_state_to_steps.at(new_state))
            {
              map_state_to_steps[new_state] = new_steps;
              to_visit.push_back(new_state);
            }
          }
        }
      }
    }
  }

  return best_steps;
}

void solver<DAY, 2>::solve(const char* input, char* output)
{
  map_type map = read_map(input);
  map.at({ 39, 39 }) = '@';
  map.at({ 39, 40 }) = '#';
  map.at({ 39, 41 }) = '@';
  map.at({ 40, 39 }) = '#';
  map.at({ 40, 40 }) = '#';
  map.at({ 40, 41 }) = '#';
  map.at({ 41, 39 }) = '@';
  map.at({ 41, 40 }) = '#';
  map.at({ 41, 41 }) = '@';
  {
    while (true)
    {
      eliminate_dead_ends(map);
      bool eliminated_doors = eliminate_dead_end_doors(map);
      if (eliminated_doors == false)
      {
        break;
      }
    }
    mark_intersections(map);
    map.data.push_back(0);
    output += sprintf(output, "%s\n\n", map.data.data());
  }

  output += sprintf(output, "%llu\n", solve_2(map));
}

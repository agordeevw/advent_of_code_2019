#include <algorithm>
#include <cassert>
#include <vector>

#include <unordered_map>
#include <unordered_set>
#include "solver.hpp"
#include "common/vec2.hpp"

constexpr int DAY = 24;

namespace
{
struct map
{
  map() : data(width * height, '.')
  {
  }
  map(map const& other) : data(other.data)
  {
  }
  map& operator=(map const& other)
  {
    if (this != &other)
    {
      data = other.data;
    }
    return *this;
  }

  char& at(int x, int y)
  {
    return data[x + y * width];
  }

  char at(int x, int y) const
  {
    return data[x + y * width];
  }

  static bool in_bounds(int x, int y)
  {
    return x >= 0 && x < width && y >= 0 && y < height;
  }

  static bool in_bounds_2(int x, int y)
  {
    return x >= 0 && x < width && y >= 0 && y < height;
  }

  static const int width = 5;
  static const int height = 5;
  std::vector<char> data;
};

map process_map(map const& m)
{
  map ret = m;
  for (int y = 0; y < m.height; y++)
  {
    for (int x = 0; x < m.width; x++)
    {
      int adjacent_bugs = 0;
      if (map::in_bounds(x + 1, y) && m.at(x + 1, y) == '#') adjacent_bugs++;
      if (map::in_bounds(x - 1, y) && m.at(x - 1, y) == '#') adjacent_bugs++;
      if (map::in_bounds(x, y + 1) && m.at(x, y + 1) == '#') adjacent_bugs++;
      if (map::in_bounds(x, y - 1) && m.at(x, y - 1) == '#') adjacent_bugs++;
      if (m.at(x, y) == '#')
      {
        ret.at(x, y) = (adjacent_bugs == 1) ? '#' : '.';
      }
      else if (m.at(x, y) == '.')
      {
        ret.at(x, y) = (adjacent_bugs == 1 || adjacent_bugs == 2) ? '#' : '.';
      }
    }
  }

  return ret;
}

uint64_t hash_map(map const& m)
{
  uint64_t ret = 0;
  uint64_t i = 0;
  for (int y = 0; y < m.height; y++)
  {
    for (int x = 0; x < m.width; x++)
    {
      if (m.at(x, y) == '#')
      {
        ret |= (1ull << i);
      }
      i++;
    }
  }
  return ret;
}

using map_hierarchy = std::unordered_map<int, map>;

static const int max_level = 299;
static const int min_level = -299;

struct tile
{
  int level;
  int x;
  int y;
};

std::vector<tile> adjacent_tiles(tile const& t)
{
  assert(t.x != 2 || t.y != 2);
  std::vector<tile> ret;

  if (map::in_bounds(t.x + 1, t.y))
  {
    if (t.x + 1 == 2 && t.y == 2)
    {
      ret.push_back({ t.level + 1, 0, 0 });
      ret.push_back({ t.level + 1, 0, 1 });
      ret.push_back({ t.level + 1, 0, 2 });
      ret.push_back({ t.level + 1, 0, 3 });
      ret.push_back({ t.level + 1, 0, 4 });
    }
    else
    {
      ret.push_back({ t.level, t.x + 1, t.y });
    }
  }
  else
  {
    ret.push_back({ t.level - 1, 3, 2 });
  }

  if (map::in_bounds(t.x - 1, t.y))
  {
    if (t.x - 1 == 2 && t.y == 2)
    {
      ret.push_back({ t.level + 1, 4, 0 });
      ret.push_back({ t.level + 1, 4, 1 });
      ret.push_back({ t.level + 1, 4, 2 });
      ret.push_back({ t.level + 1, 4, 3 });
      ret.push_back({ t.level + 1, 4, 4 });
    }
    else
    {
      ret.push_back({ t.level, t.x - 1, t.y });
    }
  }
  else
  {
    ret.push_back({ t.level - 1, 1, 2 });
  }

  if (map::in_bounds(t.x, t.y + 1))
  {
    if (t.x == 2 && t.y + 1 == 2)
    {
      ret.push_back({ t.level + 1, 0, 0 });
      ret.push_back({ t.level + 1, 1, 0 });
      ret.push_back({ t.level + 1, 2, 0 });
      ret.push_back({ t.level + 1, 3, 0 });
      ret.push_back({ t.level + 1, 4, 0 });
    }
    else
    {
      ret.push_back({ t.level, t.x, t.y + 1 });
    }
  }
  else
  {
    ret.push_back({ t.level - 1, 2, 3 });
  }

  if (map::in_bounds(t.x, t.y - 1))
  {
    if (t.x == 2 && t.y - 1 == 2)
    {
      ret.push_back({ t.level + 1, 0, 4 });
      ret.push_back({ t.level + 1, 1, 4 });
      ret.push_back({ t.level + 1, 2, 4 });
      ret.push_back({ t.level + 1, 3, 4 });
      ret.push_back({ t.level + 1, 4, 4 });
    }
    else
    {
      ret.push_back({ t.level, t.x, t.y - 1 });
    }
  }
  else
  {
    ret.push_back({ t.level - 1, 2, 1 });
  }

  return ret;
}

map_hierarchy process_map_hierarchy(map_hierarchy const& mh)
{
  map_hierarchy ret = mh;
  for (int level = min_level; level <= max_level; level++)
  {
    for (int y = 0; y < 5; y++)
    {
      for (int x = 0; x < 5; x++)
      {
        if (x == 2 && y == 2)
        {
          continue;
        }
        std::vector<tile> adj = adjacent_tiles({ level, x, y });
        uint64_t adj_bugs = 0;
        for (auto const& t : adj)
        {
          if (mh.at(t.level).at(t.x, t.y) == '#')
          {
            adj_bugs++;
          }
        }
        if (mh.at(level).at(x, y) == '#')
        {
          ret.at(level).at(x, y) = (adj_bugs == 1) ? '#' : '.';
        }
        else if (mh.at(level).at(x, y) == '.')
        {
          ret.at(level).at(x, y) = (adj_bugs == 1 || adj_bugs == 2) ? '#' : '.';
        }
      }
    }
  }

  return ret;
}

uint64_t count_bugs_at_level(map const& m)
{
  uint64_t ret = 0;
  for (int y = 0; y < 5; y++)
  {
    for (int x = 0; x < 5; x++)
    {
      if (m.at(x, y) == '#')
      {
        ret++;
      }
    }
  }
  return ret;
}

uint64_t count_bugs(map_hierarchy const& mh)
{
  uint64_t ret = 0;
  for (auto const& p : mh)
  {
    ret += count_bugs_at_level(p.second);
  }
  return ret;
}

} // namespace

void solver<DAY, 1>::solve(const char* input, char* output)
{
  std::unordered_set<uint64_t> hashes;

  map m;
  for (int y = 0; y < 5; y++)
  {
    for (int x = 0; x < 5; x++)
    {
      m.data[x + 5 * y] = input[x + 6 * y];
    }
  }
  while (hashes.find(hash_map(m)) == hashes.end())
  {
    hashes.emplace(hash_map(m));
    m = process_map(m);
  }

  output += sprintf(output, "%llu", hash_map(m));
}

void solver<DAY, 2>::solve(const char* input, char* output)
{
  map_hierarchy mh;
  mh[0] = {};
  for (int y = 0; y < 5; y++)
  {
    for (int x = 0; x < 5; x++)
    {
      mh.at(0).at(x, y) = input[x + 6 * y];
    }
  }
  for (int i = 1; i <= max_level + 1; i++)
  {
    mh[i] = {};
    mh.at(i).at(2, 2) = '?';
  }
  for (int i = -1; i >= min_level - 1; i--)
  {
    mh[i] = {};
    mh.at(i).at(2, 2) = '?';
  }

  for (int i = 0; i < 200; i++)
  {
    mh = process_map_hierarchy(mh);
  }
  output += sprintf(output, "%llu", count_bugs(mh));
}

#include <algorithm>
#include <cassert>
#include <vector>

#include "solver.hpp"

constexpr int DAY = 10;
namespace 
{

struct map
{
  char at(int x, int y) const
  {
    assert(x >= 0 && x < width);
    assert(y >= 0 && y < height);
    return data[x + y * width];
  }

  char& at(int x, int y)
  {
    assert(x >= 0 && x < width);
    assert(y >= 0 && y < height);
    return data[x + y * width];
  }

  bool is_inside(int x, int y) const
  {
    return x >= 0 && x < width && y >= 0 && y < height;
  }

  int width = 0;
  int height = 0;
  std::vector<char> data;
};

static map read_map(const char* input)
{
  map map;
  {
    const char* s = input;
    map.width = 0;
    while (*s++ != '\n')
    {
      map.width++;
    }
  }
  while (*input != 0)
  {
    while (*input != '\n')
    {
      map.data.push_back(*input);
      input++;
    }
    map.height++;
    input++;
  }
  return map;
}

struct vec2
{
  int x;
  int y;
};

static bool operator==(vec2 a, vec2 b)
{
  return a.x == b.x && a.y == b.y;
}

static std::vector<vec2> get_asteroid_positions(map const& map)
{
  std::vector<vec2> ret;

  for (int y = 0; y < map.height; y++)
  {
    for (int x = 0; x < map.height; x++)
    {
      if (map.at(x, y) == '#')
      {
        ret.push_back({ x, y });
      }
    }
  }

  return ret;
}

static int gcd(int a, int b)
{
  if (a == 0) return b;
  if (b == 0) return a;
  return a > b ? gcd(a % b, b) : gcd(a, b % a);
}

static vec2 dir(vec2 from, vec2 to)
{
  vec2 dir = { to.x - from.x, to.y - from.y };
  int g = gcd(abs(dir.x), abs(dir.y));
  dir.x /= g;
  dir.y /= g;
  return dir;
}

static int count_visible_asteroids(map const& base_map, std::vector<vec2> const& base_asteroids, vec2 center)
{
  map m = base_map;
  std::vector<vec2> asteroids = base_asteroids;
  // sort by angle
  std::sort(asteroids.begin(), asteroids.end(), [center](vec2 const& lhs, vec2 const& rhs)
  {
    if (lhs.x == center.x && lhs.y == center.y)
    {
      return true;
    }
    if (rhs.x == center.x && rhs.y == center.y)
    {
      return false;
    }

    vec2 lhs_dir = dir(center, lhs);
    vec2 rhs_dir = dir(center, rhs);
    double lhs_angle = std::atan2(lhs_dir.x, lhs_dir.y);
    double rhs_angle = std::atan2(rhs_dir.x, rhs_dir.y);
    return lhs_angle < rhs_angle;
  });
  auto new_end = std::unique(asteroids.begin() + 1, asteroids.end(), [center](vec2 const& lhs, vec2 const& rhs)
  {
    vec2 lhs_dir = dir(center, lhs);
    vec2 rhs_dir = dir(center, rhs);
    return lhs_dir.x == rhs_dir.x && lhs_dir.y == rhs_dir.y;
  });
  return (int)(new_end - asteroids.begin()) - 1;
}

}
void solver<DAY, 1>::solve(const char* input, char* output)
{
  map base_map = read_map(input);
  std::vector<vec2> asteroids = get_asteroid_positions(base_map);
  int best_visible_asteroids = 0;
  vec2 p;
  uint64_t point_id = 0;
  for (vec2 asteroid : asteroids)
  {
    int num_visible_asteroids = count_visible_asteroids(base_map, asteroids, asteroid);
    if (num_visible_asteroids > best_visible_asteroids)
    {
      best_visible_asteroids = num_visible_asteroids;
      p = asteroid;
    }
    point_id++;
  }
  sprintf(output, "%d, (%d, %d)", best_visible_asteroids, p.x, p.y);
}

void solver<DAY, 2>::solve(const char* input, char* output)
{
  vec2 center{ 23, 20 };
  map base_map = read_map(input);
  std::vector<vec2> asteroids = get_asteroid_positions(base_map);
  std::sort(asteroids.begin(), asteroids.end(), [center](vec2 const& lhs, vec2 const& rhs)
  {
    if (lhs.x == center.x && lhs.y == center.y)
    {
      return true;
    }
    if (rhs.x == center.x && rhs.y == center.y)
    {
      return false;
    }

    vec2 lhs_dir = dir(center, lhs);
    vec2 rhs_dir = dir(center, rhs);
    double lhs_angle = std::atan2(-lhs_dir.x, lhs_dir.y);
    double rhs_angle = std::atan2(-rhs_dir.x, rhs_dir.y);
    return lhs_angle < rhs_angle;
  });
  int num_on_vert_dir = 0;
  for (auto asteroid : asteroids)
  {
    if (asteroid.y < center.y && asteroid.x == center.x)
    {
      num_on_vert_dir++;
    }
  }
  std::rotate(asteroids.begin(), asteroids.begin() + asteroids.size() - num_on_vert_dir, asteroids.end());
  // asteroids sorted by angle
  // group by angle
  std::vector<vec2> dirs;
  {
    std::vector<vec2> tmp_asteroids = asteroids;
    auto new_end = std::unique(tmp_asteroids.begin(), tmp_asteroids.end(), [center](vec2 const& lhs, vec2 const& rhs)
    {
      vec2 lhs_dir = dir(center, lhs);
      vec2 rhs_dir = dir(center, rhs);
      return dir(center, lhs) == dir(center, rhs);
    });
    tmp_asteroids.resize(new_end - tmp_asteroids.begin());
    dirs.resize(tmp_asteroids.size());
    for (int i = 0; i < tmp_asteroids.size(); i++)
    {
      dirs[i] = dir(center, tmp_asteroids[i]);
    }
  }
  std::vector<std::vector<vec2>> angle_groups(dirs.size());
  {
    for (int i = 0; i < dirs.size(); i++)
    {
      for (int j = 0; j < asteroids.size(); j++)
      {
        if (dirs[i] == dir(center, asteroids[j]))
        {
          angle_groups[i].push_back(asteroids[j]);
        }
      }
      std::sort(angle_groups[i].begin(), angle_groups[i].end(), [center](vec2 a, vec2 b)
      {
        vec2 da{ a.x - center.x, a.y - center.y };
        vec2 db{ b.x - center.x, b.y - center.y };
        return da.x * da.x + da.y * da.y > db.x * db.x + db.y * db.y;
      });
    }
  }

  vec2 p = {};
  int dir_id = 0;
  int i = 0;
  while (i < 200)
  {
    if (angle_groups[dir_id].size() > 0)
    {
      p = angle_groups[dir_id].back();
      angle_groups[dir_id].pop_back();
      i++;
    }
    dir_id = (dir_id + 1) % angle_groups.size();
  }

  sprintf(output, "%d %d", p.x, p.y);
}

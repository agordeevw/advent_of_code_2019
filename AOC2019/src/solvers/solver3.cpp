#include <algorithm>
#include <vector>

#include "solver.hpp"
#include "common/vec2.hpp"

constexpr int DAY = 3;

namespace
{
bool intersect(vec2 al, vec2 ar, vec2 bl, vec2 br, vec2& intersection)
{
  bool a_horiz = al.y == ar.y;
  if (a_horiz)
  {
    int lx = al.x;
    int rx = ar.x;
    al.x = lx < rx ? lx : rx;
    ar.x = lx >= rx ? lx : rx;
  }
  else
  {
    int uy = al.y;
    int dy = ar.y;
    al.y = uy < dy ? uy : dy;
    ar.y = uy >= dy ? uy : dy;
  }

  bool b_horiz = bl.y == br.y;
  if (b_horiz)
  {
    int lx = bl.x;
    int rx = br.x;
    bl.x = lx < rx ? lx : rx;
    br.x = lx >= rx ? lx : rx;
  }
  else
  {
    int uy = bl.y;
    int dy = br.y;
    bl.y = uy < dy ? uy : dy;
    br.y = uy >= dy ? uy : dy;
  }

  if (a_horiz && b_horiz)
  {
    if (al.y != bl.y || al.x > br.x || bl.x > ar.x)
    {
      return false;
    }

    // intersection segment
    int il = 0, ir = 0;
    if (bl.x >= al.x && bl.x <= ar.x)
    {
      il = bl.x;
      ir = std::min(ar.x, br.x);
    }
    if (al.x >= bl.x && al.x <= br.x)
    {
      il = al.x;
      ir = std::min(ar.x, br.x);
    }
    intersection.y = al.y;
    if (il <= 0 && ir <= 0)
    {
      intersection.x = std::max(il, ir);
    }
    else if (il >= 0 && ir >= 0)
    {
      intersection.x = std::min(il, ir);
    }
    else
    {
      intersection.x = 0;
    }

    return true;
  }

  if (!a_horiz && !b_horiz)
  {
    if (al.x != bl.x || al.y > br.y || bl.y > ar.y)
    {
      return false;
    }

    // intersection segment
    int il = 0, ir = 0;
    if (bl.y >= al.y && bl.y <= ar.y)
    {
      il = bl.y;
      ir = std::min(ar.y, br.y);
    }
    if (al.y >= bl.y && al.y <= br.y)
    {
      il = al.y;
      ir = std::min(ar.y, br.y);
    }
    intersection.x = al.x;
    if (il <= 0 && ir <= 0)
    {
      intersection.y = std::max(il, ir);
    }
    else if (il >= 0 && ir >= 0)
    {
      intersection.y = std::min(il, ir);
    }
    else
    {
      intersection.y = 0;
    }

    return true;
  }

  if (a_horiz)
  {
    int x = bl.x;
    int y = al.y;
    if (x >= al.x && x <= ar.x && y >= bl.y && y <= br.y)
    {
      intersection.x = x;
      intersection.y = y;
      return true;
    }
  }
  else
  {
    int x = al.x;
    int y = bl.y;
    if (x >= bl.x && x <= br.x && y >= al.y && y <= ar.y)
    {
      intersection.x = x;
      intersection.y = y;
      return true;
    }
  }

  return false;
}

int dist_l1(vec2 const& a, vec2 const& b)
{
  return abs(a.x - b.x) + abs(a.y - b.y);
}

std::vector<vec2> read_wire(const char* s)
{
  std::vector<vec2> ret;
  vec2 p{ 0, 0 };
  ret.push_back(p);
  while (*s != '\n' && *s != '\0')
  {
    char dir;
    int dist;
    sscanf(s, "%1c%d", &dir, &dist);
    switch (dir)
    {
      case 'R':
      p.x += dist;
      break;
      case 'L':
      p.x -= dist;
      break;
      case 'U':
      p.y += dist;
      break;
      case 'D':
      p.y -= dist;
      break;
    }
    ret.push_back(p);
    while (*s != ',' && *s != '\n' && *s != '\0') s++;
    if (*s == ',') s++;
  }

  return ret;
}

} // namespace

void solver<DAY, 1>::solve(const char* input, char* output)
{
  std::vector<vec2> wire_a = read_wire(input);
  while (*input != '\n') input++;
  input++;
  std::vector<vec2> wire_b = read_wire(input);

  int best_dist = INT_MAX;
  for (uint32_t i = 0; i < wire_a.size() - 1; i++)
  {
    for (uint32_t j = 0; j < wire_b.size() - 1; j++)
    {
      vec2 intersection;
      if (intersect(wire_a[i], wire_a[i + 1], wire_b[j], wire_b[j + 1], intersection) && intersection.x != 0 && intersection.y != 0)
      {
        int dist = dist_l1(intersection, vec2{ 0, 0 });
        best_dist = dist < best_dist ? dist : best_dist;
      }
    }
  }

  sprintf(output, "%d", best_dist);
}

void solver<DAY, 2>::solve(const char* input, char* output)
{
  std::vector<vec2> wire_a = read_wire(input);
  while (*input != '\n') input++;
  input++;
  std::vector<vec2> wire_b = read_wire(input);

  int best_steps = INT_MAX;
  int wire_a_steps = 0;
  for (uint32_t i = 0; i < wire_a.size() - 1; i++)
  {
    int wire_b_steps = 0;
    for (uint32_t j = 0; j < wire_b.size() - 1; j++)
    {
      vec2 intersection;
      if (intersect(wire_a[i], wire_a[i + 1], wire_b[j], wire_b[j + 1], intersection) && intersection.x != 0 && intersection.y != 0)
      {
        int wire_a_add_steps = dist_l1(wire_a[i], intersection);
        int wire_b_add_steps = dist_l1(wire_b[j], intersection);
        int steps = wire_a_steps + wire_a_add_steps + wire_b_steps + wire_b_add_steps;
        best_steps = steps < best_steps ? steps : best_steps;
      }
      wire_b_steps += dist_l1(wire_b[j], wire_b[j + 1]);
    }
    wire_a_steps += dist_l1(wire_a[i], wire_a[i + 1]);
  }

  sprintf(output, "%d", best_steps);
}

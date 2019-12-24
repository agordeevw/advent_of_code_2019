#include <algorithm>
#include <cassert>
#include <vector>

#include "solver.hpp"

constexpr int DAY = 12;
namespace
{

struct planet_system
{
  int p[4][3];
  int v[4][3];
};

static inline bool operator==(planet_system const& l, planet_system const&r)
{
  for (int i = 0; i < 4; i++)
  {
    for (int d = 0; d < 3; d++)
    {
      if (l.p[i][d] != r.p[i][d]) return false;
      if (l.v[i][d] != r.v[i][d]) return false;
    }
  }
  return true;
}

static inline bool operator!=(planet_system const& l, planet_system const& r)
{
  return !(l == r);
}

static int sign(int x)
{
  return x == 0 ? 0 : (x > 0 ? 1 : -1);
}

planet_system simulate_step(planet_system const& s)
{
  planet_system ret = s;
  for (int i = 0; i < 4; i++)
  {
    for (int j = i + 1; j < 4; j++)
    {
      for (int d = 0; d < 3; d++)
      {
        int change = sign(ret.p[j][d] - ret.p[i][d]);
        ret.v[i][d] += change;
        ret.v[j][d] -= change;
      }
    }
  }
  for (int i = 0; i < 4; i++)
  {
    for (int d = 0; d < 3; d++)
    {
      ret.p[i][d] += ret.v[i][d];
    }
  }
  return ret;
}

int64_t calc_energy(planet_system const& s)
{
  int64_t energy = 0;
  for (int i = 0; i < 4; i++)
  {
    int64_t pot = 0;
    int64_t kin = 0;
    for (int d = 0; d < 3; d++)
    {
      pot += std::abs(s.p[i][d]);
      kin += std::abs(s.v[i][d]);
    }
    energy += pot * kin;
  }
  return energy;
}

static int64_t gcd(int64_t a, int64_t b)
{
  if (a == 0) return b;
  if (b == 0) return a;
  return (a > b) ? gcd(a % b, b) : gcd(a, b % a);
}

static int64_t lcm(int64_t a, int64_t b)
{
  return a * b / gcd(a, b);
}

}
void solver<DAY, 1>::solve(const char* input, char* output)
{
  planet_system s;
  for (int i = 0; i < 4; i++)
  {
    sscanf(input, "<x=%d, y=%d, z=%d>", &s.p[i][0], &s.p[i][1], &s.p[i][2]);
    s.v[i][0] = 0;
    s.v[i][1] = 0;
    s.v[i][2] = 0;
    while (*input != '\n' && *input != 0)
    {
      input++;
    }
    if (*input == '\n')
    {
      input++;
    }
  }
  planet_system initial_s;
  initial_s = s;

  int64_t num_steps[3] = { 0, 0, 0 };
  bool period_found[3] = { false, false, false };
  do
  {
    s = simulate_step(s);
    for (int d = 0; d < 3; d++)
    {
      if (!period_found[d]) num_steps[d]++;
    }
    for (int d = 0; d < 3; d++)
    {
      if (period_found[d]) continue;
      period_found[d] = true;
      for (int i = 0; i < 4; i++)
      {
        if (!(s.p[i][d] == initial_s.p[i][d] && s.v[i][d] == initial_s.v[i][d]))
        {
          period_found[d] = false;
          break;
        }
      }
      if (period_found[d])
      {
        printf("period %d\n", d);
      }
    }
  } while (!(period_found[0] && period_found[1] && period_found[2]));

  sprintf(output, "%lld", lcm(lcm(num_steps[0], num_steps[1]), num_steps[2]));
}

void solver<DAY, 2>::solve(const char* input, char* output)
{
  (void)input;
  (void)output;
  // Got lazy, solved in prev function
}

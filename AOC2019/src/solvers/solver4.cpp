#include <algorithm>
#include <vector>

#include "solver.hpp"

constexpr int DAY = 4;

namespace
{

static bool is_password(int x)
{
  int digits[6];
  int bases[6] = { 1, 10, 100, 1000, 10000, 100000 };
  for (int i = 0; i < 6; i++)
  {
    digits[5 - i] = (x / bases[i]) % 10;
  }

  bool is_two_adjacent_same = false;
  bool is_increasing_sequence = true;
  {
    for (int i = 0; i < 5; i++)
    {
      if (digits[i] == digits[i + 1])
      {
        is_two_adjacent_same = true;
      }
      if (digits[i] > digits[i + 1])
      {
        is_increasing_sequence = false;
      }
    }
  }

  return is_two_adjacent_same && is_increasing_sequence;
}

static bool is_password_2(int x)
{
  int digits[6];
  int bases[6] = { 1, 10, 100, 1000, 10000, 100000 };
  for (int i = 0; i < 6; i++)
  {
    digits[5 - i] = (x / bases[i]) % 10;
  }

  bool is_two_adjacent_same = false;
  bool is_increasing_sequence = true;
  {
    for (int i = 0; i < 5; i++)
    {
      if (digits[i] == digits[i + 1])
      {
        is_two_adjacent_same = true;
      }
      if (digits[i] > digits[i + 1])
      {
        is_increasing_sequence = false;
      }
    }
  }

  if (is_two_adjacent_same && is_increasing_sequence)
  {
    int num_digits[10];
    for (int i = 0; i < 10; i++)
    {
      num_digits[i] = 0;
    }
    for (int i = 0; i < 6; i++)
    {
      num_digits[digits[i]]++;
    }
    for (int i = 0; i < 10; i++)
    {
      if (num_digits[i] == 2)
        return true;
    }
  }

  return false;
}

} // namespace

void solver<DAY, 1>::solve(const char* input, char* output)
{
  output[0] = 0;

  int range_min;
  int range_max;
  sscanf(input, "%d-%d", &range_min, &range_max);

  int num_passwords = 0;
  for (int x = range_min; x <= range_max; x++)
  {
    if (is_password(x))
    {
      num_passwords++;
    }
  }

  sprintf(output, "%d", num_passwords);
}

void solver<DAY, 2>::solve(const char* input, char* output)
{
  output[0] = 0;

  int range_min;
  int range_max;
  sscanf(input, "%d-%d", &range_min, &range_max);

  int num_passwords = 0;
  for (int x = range_min; x <= range_max; x++)
  {
    if (is_password_2(x))
    {
      num_passwords++;
    }
  }

  sprintf(output, "%d", num_passwords);
}
#include <algorithm>
#include <cassert>
#include <vector>

#include "solver.hpp"

constexpr int DAY = 22;

namespace
{
void deal_into_new_stack(std::vector<int64_t>& deck)
{
  std::reverse(deck.begin(), deck.end());
}

void cut_n(int64_t n, std::vector<int64_t>& deck)
{
  if (n < 0)
  {
    n += deck.size();
  }
  std::rotate(deck.begin(), deck.begin() + n, deck.end());
}

void deal_with_increment_n(int64_t n, std::vector<int64_t>& deck)
{
  std::vector<int64_t> tmp_deck = deck;
  int64_t dst = 0;
  for (uint64_t src = 0; src < deck.size(); src++)
  {
    deck[dst % deck.size()] = tmp_deck[src];
    dst += n;
  }
}

struct command
{
  int type;
  int arg;
};

static constexpr uint64_t m = 119315717514047;

constexpr uint64_t mod_add(uint64_t x, uint64_t y)
{
  return (x + y) % m;
}

constexpr uint64_t mod_sub(uint64_t x, uint64_t y)
{
  return ((x + m) - y) % m;
}

constexpr uint64_t mod_mul(uint64_t x, uint64_t y)
{
  uint64_t ret = 0;
  for (int i = 0; i < 63; i++)
  {
    if ((x >> i) & 1)
    {
      uint64_t summand = y;
      for (int j = 0; j < i; j++)
      {
        summand = (summand << 1) % m;
      }

      ret = mod_add(ret, summand);
    }
  }
  return ret;
}

static_assert(mod_mul(741276321786329, 127362179045133) == 2271970138446, "");

constexpr uint64_t mod_exp(uint64_t x, uint64_t y)
{
  int64_t ret = 1;
  int64_t a = x % m;
  for (int i = 0; i < 63; i++)
  {
    if ((y >> i) & 1)
    {
      ret = mod_mul(ret, a);
    }
    a = mod_mul(a, a);
  }
  return ret;
}

constexpr int64_t mod_inv(int64_t x)
{
  return mod_exp(x, m - 2);
}

// Put solution helpers here
} // namespace

void solver<DAY, 1>::solve(const char* input, char* output)
{
  std::vector<int64_t> deck(10007);
  for (uint64_t i = 0; i < deck.size(); i++)
  {
    deck[i] = i;
  }

  std::vector<command> commands;

  while (*input)
  {
    int val;
    if (sscanf(input, "deal with increment %d\n", &val) == 1)
    {
      command c;
      c.type = 0;
      c.arg = val;
      commands.push_back(c);
    }
    else if (sscanf(input, "cut %d\n", &val) == 1)
    {
      command c;
      c.type = 1;
      c.arg = val;
      commands.push_back(c);
    }
    else
    {
      command c;
      c.type = 2;
      commands.push_back(c);
    }
    while (*input++ != '\n');
  }

  for (uint64_t i = 0; i < deck.size(); i++)
  {
    if (deck[i] == 2019)
    {
      output += sprintf(output, "%llu", i);
      break;
    }
  }
}

void solver<DAY, 2>::solve(const char* input, char* output)
{
  std::vector<command> commands;
  while (*input)
  {
    int val;
    if (sscanf(input, "deal with increment %d\n", &val) == 1)
    {
      command c;
      c.type = 0;
      c.arg = val;
      commands.push_back(c);
    }
    else if (sscanf(input, "cut %d\n", &val) == 1)
    {
      command c;
      c.type = 1;
      c.arg = val;
      commands.push_back(c);
    }
    else
    {
      command c;
      c.type = 2;
      commands.push_back(c);
    }
    while (*input++ != '\n');
  }

  struct linfunc
  {
    uint64_t a;
    uint64_t b;

    uint64_t eval(uint64_t x)
    {
      uint64_t ret = mod_add(mod_mul(a, x), b);
      return ret;
    }

    linfunc apply_to(linfunc const& other)
    {
      uint64_t new_a = mod_mul(a, other.a);
      uint64_t new_b = mod_add(mod_mul(a, other.b), b);
      return {new_a, new_b};
    }
  };

  linfunc shuffle = { 1, 0 };
  for (command c : commands)
  {
    if (c.type == 2) // deal into new stack
    {
      shuffle = linfunc{ m - 1, m - 1 }.apply_to(shuffle);
    }
    else if (c.type == 1) // cut n
    {
      shuffle = linfunc{ 1, (uint64_t((int64_t)m - c.arg)) }.apply_to(shuffle);
    }
    else if (c.type == 0) // deal with increment n
    {
      shuffle = linfunc{ (uint64_t)c.arg, 0 }.apply_to(shuffle);
    }
  }

  constexpr int64_t n = 101741582076661;

  output += sprintf(output, "a=%lld, b=%lld\n", shuffle.a, shuffle.b);

  uint64_t a = shuffle.a;
  uint64_t b = shuffle.b;
  uint64_t a_to_n = mod_exp(a, n);
  uint64_t ret =  
    mod_mul(
      mod_sub(
        2020,
        mod_mul(
          b,
          mod_mul(
            a_to_n - 1,
            mod_inv(a - 1)
          )
        )
      ),
      mod_inv(a_to_n)
    );
  output += sprintf(output, "ret=%lld\n", ret);
}

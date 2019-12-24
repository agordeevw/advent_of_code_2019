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
  uint64_t dst = 0;
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

const uint64_t d = 119315717514047;

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

  const uint64_t deck_size = 119315717514047;
  const uint64_t commands_repeat = 101741582076661;

  uint64_t period = 0;
  uint64_t target_card_pos = 2020;
  for (uint64_t i = 0; i < commands_repeat; i++)
  {
    for (command c : commands)
    {
      if (c.type == 2)
      {
        target_card_pos = deck_size - 1 - target_card_pos;
      }
      else if (c.type == 1)
      {
        target_card_pos = (target_card_pos + deck_size - c.arg) % deck_size;
      }
      else if (c.type == 0)
      {
        uint64_t p = 0;
        for (int j = 0; j < c.arg; j++)
        {
          p = (p + target_card_pos) % deck_size;
        }
        target_card_pos = p;
      }
    }
    if (period > 0 && target_card_pos == 2020)
    {
      printf("PERIOD: %llu\n", period);
    }
    period++;
  }

  output += sprintf(output, "%llu", target_card_pos);
}

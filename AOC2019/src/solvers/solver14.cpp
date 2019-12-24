#include <algorithm>
#include <cctype>
#include <cassert>
#include <unordered_map>
#include <string>
#include <vector>

#include "solver.hpp"

constexpr int DAY = 14;
namespace
{

struct chemical_batch
{
  chemical_batch() : name(), quantity(0)
  {
  }
  chemical_batch(std::string name, int64_t quantity)
    : name(std::move(name)), quantity(quantity)
  {
  }

  std::string name;
  int64_t quantity;
};

struct reaction
{
  std::vector<chemical_batch> input;
  chemical_batch output;
};

std::unordered_map<std::string, reaction> read_reactions(const char* input)
{
  std::unordered_map<std::string, reaction> reactions;
  while (*input)
  {
    reaction reac;
    while (true)
    {
      chemical_batch batch;
      sscanf(input, "%lld", &batch.quantity);
      while (isdigit(*input)) input++;
      input++;
      while (isalpha(*input))
      {
        batch.name += *input;
        input++;
      }

      reac.input.push_back(batch);
      if (*input == ',')
      {
        input += 2; // ", "
        continue;
      }
      else if (*input == ' ') // " => "
      {
        input += 4;
        break;
      }
      else
      {
        assert(false);
      }
    }
    chemical_batch batch;
    sscanf(input, "%lld", &batch.quantity);
    while (isdigit(*input)) input++;
    input++;
    while (isalpha(*input))
    {
      batch.name += *input;
      input++;
    }
    input++; // '\n'

    reac.output = batch;

    reactions.emplace(reac.output.name, reac);
  }
  return reactions;
}

int64_t find_chemical_order(std::unordered_map<std::string, reaction> const& reactions,
                            std::unordered_map<std::string, int64_t>& orders,
                            std::string const& name)
{
  if (name == "ORE")
  {
    orders["ORE"] = 0;
    return 0;
  }
  reaction reac = reactions.at(name);
  int64_t order = 0;
  for (chemical_batch const& input_chemical : reac.input)
  {
    order = std::max(find_chemical_order(reactions, orders, input_chemical.name), order);
  }
  orders[name] = order + 1;
  return order + 1;
}

int64_t find_ore_quantity(std::unordered_map<std::string, reaction> const& reactions,
                          std::unordered_map<std::string, int64_t> const& orders,
                          int64_t fuel_quantity)
{
  std::vector<chemical_batch> chemicals;
  chemicals.emplace_back("FUEL", fuel_quantity);
  int64_t ore_quantity = 0;
  while (chemicals.size() > 0)
  {
    std::sort(chemicals.begin(), chemicals.end(), [&orders](const chemical_batch& lhs, const chemical_batch& rhs)
    {
      return orders.at(lhs.name) < orders.at(rhs.name);
    });
    chemical_batch target = chemicals.back();
    chemicals.pop_back();
    if (target.name == "ORE")
    {
      ore_quantity += target.quantity;
      continue;
    }
    reaction reac = reactions.at(target.name);
    int64_t num_reacs = (target.quantity + (reac.output.quantity - 1)) / reac.output.quantity;
    for (auto const& input_chemical : reac.input)
    {
      auto chemical_it = std::find_if(chemicals.begin(), chemicals.end(), [&input_chemical](chemical_batch const& chemical)
      {
        return chemical.name == input_chemical.name;
      });
      if (chemical_it != chemicals.end())
      {
        chemical_it->quantity += num_reacs * input_chemical.quantity;
      }
      else
      {
        chemicals.emplace_back(input_chemical.name, num_reacs * input_chemical.quantity);
      }
    }
  }
  return ore_quantity;
}

}
void solver<DAY, 1>::solve(const char* input, char* output)
{
  auto reactions = read_reactions(input);
  std::unordered_map<std::string, int64_t> orders;
  orders.emplace("ORE", 0);
  for (auto const& reac : reactions)
  {
    find_chemical_order(reactions, orders, reac.first);
  }

  sprintf(output, "%lld", find_ore_quantity(reactions, orders, 1));
}

void solver<DAY, 2>::solve(const char* input, char* output)
{
  auto reactions = read_reactions(input);
  std::unordered_map<std::string, int64_t> orders;
  orders.emplace("ORE", 0);
  for (auto const& reac : reactions)
  {
    find_chemical_order(reactions, orders, reac.first);
  }

  const int64_t max_ore_quantity = 1000000000000;
  int64_t l = 1;
  int64_t r = 5000000;
  while (l + 1 != r)
  {
    int64_t m = (l + r) / 2;
    int64_t ore_quantity = find_ore_quantity(reactions, orders, m);
    if (ore_quantity <= max_ore_quantity)
    {
      l = m;
    }
    else
    {
      r = m;
    }
  }

  sprintf(output, "%lld", l);
}

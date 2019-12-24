#include <algorithm>
#include <vector>
#include <unordered_map>

#include "solver.hpp"

constexpr int DAY = 6;
namespace
{

struct planet
{
  planet()
  {
    name[0] = 0;
  }

  explicit planet(const char* s)
  {
    strcpy(name, s);
  }

  char name[8] = { 0 };
};

bool operator==(planet const& lhs, planet const& rhs)
{
  return strcmp(lhs.name, rhs.name) == 0;
}

struct planet_hasher
{
  uint64_t operator()(planet const& p) const
  {
    return std::hash<uint64_t>{}(*(uint64_t*)(p.name));
  }
};

// Tree
// Only one parent
// Many children
// Find a sum of all heights
// COM - root

}
void solver<DAY, 1>::solve(const char* input, char* output)
{
  std::vector<planet> planets;
  std::unordered_map<planet, uint64_t, planet_hasher> map_planets_to_id;
  std::vector<uint64_t> orbit_center_ids;

  auto add_planet = [&](planet const& p)
  {
    if (map_planets_to_id.find(p) == map_planets_to_id.end())
    {
      uint64_t id = planets.size();
      map_planets_to_id.emplace(p, id);
      planets.push_back(p);
    }
  };

  // Assemble list of planets
  add_planet(planet{ "COM" });
  {
    planet orbit_center;
    planet orbiter;
    const char* tmp_input = input;
    while (*tmp_input != 0)
    {
      sscanf(tmp_input, "%3s)%3s", orbit_center.name, orbiter.name);
      tmp_input += strlen(orbit_center.name) + 1 + strlen(orbiter.name);
      if (*tmp_input == '\n')
      {
        tmp_input++;
      }

      add_planet(orbit_center);
      add_planet(orbiter);
    }
  }

  // Assemble orbiters list
  orbit_center_ids.resize(planets.size());
  orbit_center_ids[0] = (uint64_t)-1;
  {
    planet orbit_center;
    planet orbiter;
    const char* tmp_input = input;
    while (*tmp_input != 0)
    {
      sscanf(tmp_input, "%3s)%3s\n", orbit_center.name, orbiter.name);
      tmp_input += strlen(orbit_center.name) + 1 + strlen(orbiter.name);
      if (*tmp_input == '\n')
      {
        tmp_input++;
      }

      uint64_t orbit_center_id = map_planets_to_id.at(orbit_center);
      uint64_t orbiter_id = map_planets_to_id.at(orbiter);
      orbit_center_ids[orbiter_id] = orbit_center_id;
    }
  }

  uint64_t orbit_count = 0;
  for (uint64_t i = 0; i < planets.size(); i++)
  {
    uint64_t cur = i;
    while (orbit_center_ids[cur] != (uint64_t)-1)
    {
      orbit_count++;
      cur = orbit_center_ids[cur];
    }
  }

  sprintf(output, "%llu", orbit_count);
}

void solver<DAY, 2>::solve(const char* input, char* output)
{
  std::vector<planet> planets;
  std::unordered_map<planet, uint64_t, planet_hasher> map_planets_to_id;
  std::vector<uint64_t> orbit_center_ids;

  auto add_planet = [&](planet const& p)
  {
    if (map_planets_to_id.find(p) == map_planets_to_id.end())
    {
      uint64_t id = planets.size();
      map_planets_to_id.emplace(p, id);
      planets.push_back(p);
    }
  };

  // Assemble list of planets
  add_planet(planet{ "COM" });
  {
    planet orbit_center;
    planet orbiter;
    const char* tmp_input = input;
    while (*tmp_input != 0)
    {
      sscanf(tmp_input, "%3s)%3s", orbit_center.name, orbiter.name);
      tmp_input += strlen(orbit_center.name) + 1 + strlen(orbiter.name);
      if (*tmp_input == '\n')
      {
        tmp_input++;
      }

      add_planet(orbit_center);
      add_planet(orbiter);
    }
  }

  // Assemble orbiters list
  orbit_center_ids.resize(planets.size());
  orbit_center_ids[0] = (uint64_t)-1;
  {
    planet orbit_center;
    planet orbiter;
    const char* tmp_input = input;
    while (*tmp_input != 0)
    {
      sscanf(tmp_input, "%3s)%3s\n", orbit_center.name, orbiter.name);
      tmp_input += strlen(orbit_center.name) + 1 + strlen(orbiter.name);
      if (*tmp_input == '\n')
      {
        tmp_input++;
      }

      uint64_t orbit_center_id = map_planets_to_id.at(orbit_center);
      uint64_t orbiter_id = map_planets_to_id.at(orbiter);
      orbit_center_ids[orbiter_id] = orbit_center_id;
    }
  }

  uint64_t transfer_count = 0;
  {
    uint64_t you_id = map_planets_to_id.at(planet{ "YOU" });
    uint64_t san_id = map_planets_to_id.at(planet{ "SAN" });
    std::vector<uint64_t> you_path, san_path;
    while (you_id != (uint64_t)-1)
    {
      you_path.push_back(you_id);
      you_id = orbit_center_ids[you_id];
    }
    while (san_id != (uint64_t)-1)
    {
      san_path.push_back(san_id);
      san_id = orbit_center_ids[san_id];
    }
    std::reverse(you_path.begin(), you_path.end());
    std::reverse(san_path.begin(), san_path.end());
    uint64_t lca_height = 0;
    while (lca_height < you_path.size() && lca_height < san_path.size() && you_path[lca_height] == san_path[lca_height])
    {
      lca_height++;
    }
    transfer_count = (you_path.size() - lca_height - 1) + (san_path.size() - lca_height - 1);
  }

  sprintf(output, "%llu", transfer_count);
}

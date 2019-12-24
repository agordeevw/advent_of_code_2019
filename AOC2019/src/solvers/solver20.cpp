#include <algorithm>
#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "solver.hpp"
#include "common/vec2.hpp"
#include "common/a_star.hpp"

constexpr int DAY = 20;

namespace
{
struct portal_position
{
  vec2 floor_position;
  vec2 entrance_offset;
};

struct multilevel_point
{
  vec2 position;
  int level;
};

bool operator==(multilevel_point const& l, multilevel_point const& r)
{
  return l.position == r.position && l.level == r.level;
}

struct multilevel_point_hasher
{
  uint64_t operator()(multilevel_point const& p) const
  {
    return vec2_hasher{}(p.position);
  }
};

struct map_type
{
  char& at(vec2 p)
  {
    return data[p.y * (width + 1) + p.x];
  }

  char at(vec2 p) const
  {
    return data[p.y * (width + 1) + p.x];
  }

  bool in_bounds(vec2 p) const
  {
    return p.x >= 0 && p.x < width && p.y >= 0 && p.y < height;
  }

  std::vector<vec2> list_walkable_neighbours(vec2 p) const
  {
    std::vector<vec2> ret;

    const vec2 neighbours[] = { {p.x + 1, p.y}, {p.x - 1, p.y}, {p.x, p.y + 1}, {p.x, p.y - 1} };
    for (auto next : neighbours)
    {
      char symbol_at = at(next);
      if (symbol_at == '.')
      {
        ret.push_back(next);
      }
      else if (symbol_at >= 'A' && symbol_at <= 'Z')
      {
        if (portals.at("AA").back().floor_position != p
            && portals.at("ZZ").back().floor_position != p)
        {
          ret.push_back(portal_traversals.at(next));
        }
      }
    }

    return ret;
  }

  std::vector<multilevel_point> list_walkable_neighbours_multilevel(multilevel_point p) const
  {
    std::vector<multilevel_point> ret;

    const vec2 offsets[] = { { 1, 0 }, { -1,0 }, { 0, 1 }, { 0, -1 } };
    for (auto offset : offsets)
    {
      vec2 next = p.position + offset;
      char symbol_at = at(next);
      if (symbol_at == '.')
      {
        bool at_aa = portals.at("AA")[0].floor_position == next;
        bool at_zz = portals.at("ZZ")[0].floor_position == next;
        bool on_outer_walls = next.x == 2 || next.x == width - 3
          || next.y == 2 || next.y == height - 3;

        if ((p.level == 0 && (!on_outer_walls || at_aa || at_zz))
            || (p.level != 0 && (!on_outer_walls || !(at_aa || at_zz))))
        {
          ret.push_back({ next, p.level });
        }
      }
      else if (symbol_at >= 'A' && symbol_at <= 'Z')
      {
        if (portals.at("AA").back().floor_position != p.position
            && portals.at("ZZ").back().floor_position != p.position)
        {
          vec2 exit_position = portal_traversals.at(next);
          int exit_level = p.level;
          {
            std::string portal_name = portal_at_position.at(p.position);
            auto const& portal_positions = portals.at(portal_name);
            vec2 portal_0_pos = portal_positions[0].floor_position;
            int outer_layer_id = (portal_0_pos.y < 3
                                  || portal_0_pos.y > height - 4
                                  || portal_0_pos.x < 3
                                  || portal_0_pos.x > width - 4) ? 0 : 1;
            exit_level += (p.position == portal_positions[outer_layer_id].floor_position) ?
              -1 : 1;
          }
          ret.push_back({ exit_position, exit_level });
        }
      }
    }

    return ret;
  }

  void eliminate_dead_ends()
  {
    bool found_dead_end = false;
    do
    {
      found_dead_end = false;
      for (int y = 2; y < height - 2; y++)
      {
        for (int x = 2; x < width - 2; x++)
        {
          vec2 p = { x, y };
          if (at(p) == '.')
          {
            const vec2 neighbours[] = { {p.x + 1, p.y}, {p.x - 1, p.y}, {p.x, p.y + 1}, {p.x, p.y - 1} };
            int num_neighbouring_walls = 0;
            for (auto next : neighbours)
            {
              num_neighbouring_walls += (at(next) == '#') ? 1 : 0;
            }
            if (num_neighbouring_walls == 3)
            {
              found_dead_end = true;
              at(p) = '#';
            }
          }
        }
      }
    } while (found_dead_end == true);
  }

  std::vector<char> data;
  std::unordered_map<vec2, vec2, vec2_hasher> portal_traversals;
  std::unordered_map<std::string, std::vector<portal_position>> portals;
  std::unordered_map<vec2, std::string, vec2_hasher> portal_at_position;
  int width = 0;
  int height = 0;
};

static map_type read_map(const char* s)
{
  map_type map;
  map.data.resize(strlen(s) + 1);
  strcpy(map.data.data(), s);

  {
    const char* s_v = s;
    while (*s_v != '\n')
    {
      s_v++;
      map.width++;
    }
  }
  {
    const char* s_v = s;
    while (*s_v)
    {
      s_v += map.width + 1;
      map.height++;
    }
  }

  for (int y = 2; y < map.height - 2; y++)
  {
    for (int x = 2; x < map.width - 2; x++)
    {
      vec2 p = { x, y };
      char symbol = map.at(p);
      if (symbol == '.')
      {
        portal_position pos;
        bool is_portal_floor_tile = false;
        {
          const vec2 neighbours[] = { {p.x + 1, p.y}, {p.x - 1, p.y}, {p.x, p.y + 1}, {p.x, p.y - 1} };
          for (vec2 next : neighbours)
          {
            if (map.at(next) >= 'A' && map.at(next) <= 'Z')
            {
              is_portal_floor_tile = true;
              pos.floor_position = p;
              pos.entrance_offset = next - p;
              std::string portal_name;
              if (pos.entrance_offset == vec2{ 0, -1 } || pos.entrance_offset == vec2{ -1, 0 })
              {
                portal_name.push_back(map.at(p + 2 * pos.entrance_offset));
                portal_name.push_back(map.at(p + 1 * pos.entrance_offset));
              }
              else if (pos.entrance_offset == vec2{ 0, 1 } || pos.entrance_offset == vec2{ 1, 0 })
              {
                portal_name.push_back(map.at(p + 1 * pos.entrance_offset));
                portal_name.push_back(map.at(p + 2 * pos.entrance_offset));
              }
              if (map.portals.find(portal_name) == map.portals.end())
              {
                map.portals[portal_name] = {};
              }
              else
              {
                portal_position other_pos = map.portals.at(portal_name).back();
                map.portal_traversals[pos.floor_position + pos.entrance_offset] = other_pos.floor_position;
                map.portal_traversals[other_pos.floor_position + other_pos.entrance_offset] = pos.floor_position;
              }
              map.portals.at(portal_name).push_back(pos);
              map.portal_at_position[p] = portal_name;
              break;
            }
          }
        }
      }
    }
  }

  return map;
}

auto find_path(map_type const& map, vec2 start, vec2 finish)
{
  auto list_adjacent = [&map](vec2 p) -> std::vector<vec2>
  {
    return map.list_walkable_neighbours(p);
  };

  auto transition_cost = [&map](vec2, vec2) -> int64_t
  {
    return 1;
  };

  auto heuristic = [&map](vec2, vec2) -> int64_t
  {
    return 0;
  };

  return a_star<vec2, vec2_hasher, int64_t, decltype(list_adjacent), decltype(transition_cost), decltype(heuristic)>(start, finish, list_adjacent, transition_cost, heuristic);;
}

auto find_path_multilevel(map_type const& map, vec2 start, vec2 finish)
{
  auto list_adjacent = [&map](multilevel_point p) -> std::vector<multilevel_point>
  {
    return map.list_walkable_neighbours_multilevel(p);
  };

  auto transition_cost = [&map](multilevel_point, multilevel_point) -> int64_t
  {
    return 1;
  };

  auto heuristic = [&map](multilevel_point, multilevel_point) -> int64_t
  {
    return 0;
  };

  return a_star<multilevel_point, multilevel_point_hasher, int64_t, decltype(list_adjacent), decltype(transition_cost), decltype(heuristic)>({ start, 0 }, { finish, 0 }, list_adjacent, transition_cost, heuristic);;
}

} // namespace

void solver<DAY, 1>::solve(const char* input, char* output)
{
  map_type map = read_map(input);
  {
    map_type simplified_map = map;
    simplified_map.eliminate_dead_ends();
    output += sprintf(output, "Map with eliminated dead ends:\n%s\n\n", simplified_map.data.data());
  }
  uint64_t path_length = 0;
  {
    auto path = find_path(map, map.portals.at("AA")[0].floor_position, map.portals.at("ZZ")[0].floor_position);
    path_length = path.size() - 1;
    map_type marked_map = map;
    for (auto p : path)
    {
      marked_map.at(p) = '*';
    }
    output += sprintf(output, "Map with path:\n%s\n\n", marked_map.data.data());
  }
  output += sprintf(output, "Path length: %llu\n", path_length);
}

void solver<DAY, 2>::solve(const char* input, char* output)
{
  map_type map = read_map(input);
  map_type simplified_map = map;
  simplified_map.eliminate_dead_ends();
  output += sprintf(output, "%s\n\n", simplified_map.data.data());
  uint64_t path_length = 0;
  {
    output += sprintf(output, "Path:\n");
    auto path = find_path_multilevel(simplified_map, simplified_map.portals.at("AA")[0].floor_position, simplified_map.portals.at("ZZ")[0].floor_position);
    path_length = path.size() - 1;
  }
  output += sprintf(output, "Path length: %llu\n", path_length);
}

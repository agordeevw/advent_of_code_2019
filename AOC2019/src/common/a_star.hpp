#pragma once
#include <vector>
#include <unordered_map>
#include <queue>

// TODO: make no alloc a_star
// if found path is too large, store only the first segment and report how many nodes are in the full path
// limited output path -> only partial path may be reported
// frontier is limited. can't add new node -> if it's better that some other node, try to replace it, otherwise ignore it
// limited frontier -> possibility of non-optimal solution increases
// hash tables 'came_from' and 'cost_so_far' are limited
// if their limits are exceeded, tis a fuckup, should report partial path
// to reduce guesswork, it should report failures:
//   path is incomplete (out of memory for intermediate values),
//   found full path [but failed to store it completely]

// prefixing template types with t_ to see how ugly it looks

template <class t_node_type, class t_node_hasher_type, class weight_type, class t_list_adjacent_func, class t_transition_cost_func, class t_heuristic_func>
std::vector<t_node_type> a_star(t_node_type const& start, t_node_type const& finish, t_list_adjacent_func adj_func, t_transition_cost_func cost_func, t_heuristic_func heur_func)
{
  struct weighted_node_type
  {
    t_node_type node;
    weight_type weight;

    bool operator<(weighted_node_type const& other) const
    {
      // because default priority queue sorts by highest priority
      return weight > other.weight;
    }
  };

  std::priority_queue<weighted_node_type> frontier;
  frontier.push({ start, heur_func(start, finish) });
  std::unordered_map<t_node_type, weight_type, t_node_hasher_type> cost_so_far;
  std::unordered_map<t_node_type, t_node_type, t_node_hasher_type> came_from;
  cost_so_far[start] = 0;

  while (frontier.size() > 0)
  {
    weighted_node_type cur_weighted_node = frontier.top();
    t_node_type const& cur = cur_weighted_node.node;
    frontier.pop();

    if (cur == finish)
    {
      break;
    }

    const weight_type cost_at_cur = cost_so_far.at(cur);
    for (t_node_type const& next : adj_func(cur))
    {
      const weight_type new_cost = cost_at_cur + cost_func(cur, next);
      if (cost_so_far.find(next) == cost_so_far.end() || new_cost < cost_so_far.at(next))
      {
        came_from[next] = cur;
        cost_so_far[next] = new_cost;
        frontier.push({ next, new_cost + heur_func(next, finish) });
      }
    }
  }

  if (came_from.find(finish) == came_from.end())
    return {start};

  std::vector<t_node_type> path;
  path.push_back(finish);
  t_node_type p = finish;
  while (!(p == start))
  {
    p = came_from.at(p);
    path.push_back(p);
  }

  return path;
}

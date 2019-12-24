#include <algorithm>
#include <cassert>
#include <unordered_map>
#include <vector>

#include "solver.hpp"
#include "common/intcode_machine.hpp"

constexpr int DAY = 23;

namespace
{
struct packet
{
  int64_t x;
  int64_t y;
};
// Put solution helpers here
} // namespace

void solver<DAY, 1>::solve(const char* input, char* output)
{
  const unsigned int num_machines = 50;

  intcode_program program = read_intcode_program(input);
  char memory[num_machines * sizeof(intcode_machine)];
  intcode_machine* machines[num_machines];
  std::queue<packet> pending_packets[num_machines];
  for (int i = 0; i < num_machines; i++)
  {
    new(&memory[i * sizeof(intcode_machine)]) intcode_machine(program);
    machines[i] = (intcode_machine*)(&memory[i * sizeof(intcode_machine)]);
    machines[i]->push_input(i);
    machines[i]->run();
    pending_packets[i] = {};
  }
  pending_packets[255] = {};

  bool has_packet_to_target_address = false;
  int64_t ret = 0;
  while (has_packet_to_target_address == false)
  {
    // drain all packets
    for (auto machine : machines)
    {
      while (machine->has_output())
      {
        int64_t address = machine->pop_output().second;
        int64_t x = machine->pop_output().second;
        int64_t y = machine->pop_output().second;
        if (address == 255 && has_packet_to_target_address == false)
        {
          ret = y;
          has_packet_to_target_address = true;
        }
        else
        {
          pending_packets[address].push({ x, y });
        }
      }
    }

    // distribute pending packets to machines
    for (int i = 0; i < num_machines; i++)
    {
      auto machine = machines[i];
      auto& packets_for_machine = pending_packets[i];
      if (packets_for_machine.size() > 0)
      {
        while (packets_for_machine.size() > 0)
        {
          machine->push_input(packets_for_machine.front().x);
          machine->push_input(packets_for_machine.front().y);
          pending_packets[i].pop();
        }
      }
      else
      {
        machine->push_input(-1);
      }
    }

    // execute machines
    for (auto machine : machines)
    {
      machine->run();
    }
  }

  output += sprintf(output, "%lld", ret);

  for (int i = 0; i < 50; i++)
  {
    machines[i]->~intcode_machine();
  }
}

void solver<DAY, 2>::solve(const char* input, char* output)
{
  const unsigned int num_machines = 50;

  intcode_program program = read_intcode_program(input);
  char memory[num_machines * sizeof(intcode_machine)];
  intcode_machine* machines[num_machines];
  std::queue<packet> pending_packets[num_machines];
  for (int i = 0; i < num_machines; i++)
  {
    new(&memory[i * sizeof(intcode_machine)]) intcode_machine(program);
    machines[i] = (intcode_machine*)(&memory[i * sizeof(intcode_machine)]);
    machines[i]->push_input(i);
    machines[i]->run();
    pending_packets[i] = {};
  }

  std::vector<packet> packets_sent_by_nat;
  packet nat_packet = { 0, 0 };
  int64_t ret = 0;
  while (true)
  {
    bool machine_sent_packets[num_machines];
    // drain all packets
    for (int i = 0; i < num_machines; i++)
    {
      machine_sent_packets[i] = false;
      auto machine = machines[i];
      while (machine->has_output())
      {
        int64_t address = machine->pop_output().second;
        int64_t x = machine->pop_output().second;
        int64_t y = machine->pop_output().second;
        if (address == 255)
        {
          nat_packet = { x, y };
        }
        else
        {
          pending_packets[address].push({ x, y });
        }
        machine_sent_packets[i] = true;
      }
    }

    bool machine_receives_packets[num_machines];
    for (int i = 0; i < num_machines; i++)
    {
      machine_receives_packets[i] = pending_packets[i].size() > 0;
    }

    bool network_idle = true;
    for (int i = 0; i < num_machines; i++)
    {
      if (machine_sent_packets[i] || machine_receives_packets[i])
      {
        network_idle = false;
      }
    }

    if (network_idle)
    {
      packets_sent_by_nat.push_back(nat_packet);
      pending_packets[0].push(nat_packet);
      if (packets_sent_by_nat.size() > 1)
      {
        packet cur = packets_sent_by_nat[packets_sent_by_nat.size() - 1];
        packet prev = packets_sent_by_nat[packets_sent_by_nat.size() - 2];
        if (cur.y == prev.y)
        {
          ret = cur.y;
          break;
        }
      }
    }

    // distribute pending packets to machines
    for (int i = 0; i < num_machines; i++)
    {
      auto machine = machines[i];
      auto& packets_for_machine = pending_packets[i];
      if (packets_for_machine.size() > 0)
      {
        while (packets_for_machine.size() > 0)
        {
          machine->push_input(packets_for_machine.front().x);
          machine->push_input(packets_for_machine.front().y);
          pending_packets[i].pop();
        }
      }
      else
      {
        machine->push_input(-1);
      }
    }

    // execute machines
    for (auto machine : machines)
    {
      machine->run();
    }
  }

  output += sprintf(output, "%lld", ret);

  for (int i = 0; i < 50; i++)
  {
    machines[i]->~intcode_machine();
  }
}

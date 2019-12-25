#include <algorithm>
#include <cassert>
#include <vector>

#include "solver.hpp"
#include "common/vec2.hpp"
#include "common/intcode_machine.hpp"

constexpr int DAY = 25;

namespace
{
// Put solution helpers here
} // namespace

void solver<DAY, 1>::solve(const char* input, char* output)
{
  intcode_program program = read_intcode_program(input);
  intcode_machine machine{ program };
  char buffer[16 * 1024];
  uint32_t inventory_state = 0;
  while (true)
  {
    machine.run();
    while (machine.has_output())
    {
      printf("%c", (char)machine.pop_output().second);
    }
    printf("\n");

    do
    {
      fgets(buffer, sizeof(buffer), stdin);
      if (strcmp(buffer, "quit\n") == 0)
      {
        return;
      }
      else if (strcmp(buffer, "save\n") == 0)
      {
        intcode_machine_inspector inspector{ machine };

        // save program, state, ip, relative_base
        FILE* dump = fopen("outputs/machine_state", "w");
        fprintf(dump, "%llu\n", inspector.program().size());
        for (uint64_t i = 0; i < inspector.program().size(); i++)
        {
          fprintf(dump, "%lld ", inspector.program()[i]);
        }
        fprintf(dump, "\n%d\n%llu\n%llu", (int)machine.get_state(), inspector.ip(), inspector.relative_base());
        fclose(dump);
        continue;
      }
      else if (strcmp(buffer, "load\n") == 0)
      {
        intcode_program loaded_program;
        uint64_t program_size;

        FILE* dump = fopen("outputs/machine_state", "r");
        fscanf(dump, "%llu\n", &program_size);
        loaded_program.resize(program_size);
        for (uint64_t i = 0; i < program_size; i++)
        {
          fscanf(dump, "%lld ", &loaded_program[i]);
        }
        int machine_state;
        uint64_t ip;
        uint64_t relative_base;
        fscanf(dump, "\n%d\n%llu\n%llu", &machine_state, &ip, &relative_base);

        machine = intcode_machine{loaded_program};
        intcode_machine_inspector inspector{machine};

        inspector.set_ip(ip);
        inspector.set_relative_base(relative_base);
        inspector.set_execution_state((intcode_machine::execution_state)machine_state);

        fclose(dump);
        continue;
      }
      else if (strcmp(buffer, "at checkpoint 1\n") == 0)
      {
        const char* checkpoint_1_items[8]{
          "ornament",
          "easter egg",
          "hypercube",
          "hologram",
          "cake",
          "fuel cell",
          "dark matter",
          "klein bottle"
        };
        // Drop all items on the floor and repeat the command until you pass the checkpoint.
        buffer[0] = 0;
        char* buffer_view = buffer;
        uint32_t diff = (inventory_state) ^ ((inventory_state + 1) & 0xFF);
        for (int i = 0; i < 8; i++)
        {
          if ((diff >> i) & 1)
          {
            // if item on hand
            if ((inventory_state >> i) & 1)
            {
              printf("dropping %s...\n", checkpoint_1_items[i]);
              buffer_view += sprintf(buffer_view, "drop %s\n", checkpoint_1_items[i]);
            }
            else
            {
              printf("taking %s...\n", checkpoint_1_items[i]);
              buffer_view += sprintf(buffer_view, "take %s\n", checkpoint_1_items[i]);
            }
          }
        }
        inventory_state = (inventory_state + 1) & 0xFF;
        printf("going west...\n");
        buffer_view += sprintf(buffer_view, "west\n");
      }
      break;
    } while (1);
    for (int i = 0; i < strlen(buffer); i++)
    {
      machine.push_input(buffer[i]);
    }
  }
}

void solver<DAY, 2>::solve(const char* input, char* output)
{
}

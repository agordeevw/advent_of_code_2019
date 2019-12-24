#include <stdio.h>
#include <memory>

#include "solver.hpp"

class file_ptr : public std::unique_ptr<FILE, decltype(&fclose)>
{
  using base_class = std::unique_ptr<FILE, decltype(&fclose)>;

public:
  file_ptr() : base_class{ nullptr, &fclose }
  {
  }
  file_ptr(FILE* file) : base_class{ file, &fclose }
  {
  }
  ~file_ptr() = default;
};

int main(int argc, char** argv)
{
  // Arg 1 - day (1-25)
  // Arg 2 - subtask (1, 2)
  if (argc < 3)
  {
    return 1;
  }

  int day = atoi(argv[1]);
  int subtask = atoi(argv[2]);
  std::unique_ptr<base_solver> solver = create_solver(day, subtask);
  if (!solver)
  {
    return 1;
  }

  char input_filename[32] = {};
  sprintf(input_filename, "inputs/input%d.txt", day);
  file_ptr input_file{ fopen(input_filename, "r") };
  if (!input_file)
  {
    return 1;
  }

  char output_filename[32] = {};
  sprintf(output_filename, "outputs/output%d.txt", day);
  file_ptr output_file{ fopen(output_filename, "w") };
  if (!output_file)
  {
    return 1;
  }

  long input_length = 0;
  {
    fseek(input_file.get(), 0, SEEK_END);
    input_length = ftell(input_file.get());
    fseek(input_file.get(), 0, SEEK_SET);
  }

  std::unique_ptr<char[]> input_buffer{ new char[input_length + 1] };
  {
    char* buf = input_buffer.get();
    while (fgets(buf, input_length + 1, input_file.get()))
    {
      buf += strlen(buf);
    }
  }

  std::unique_ptr<char[]> output_buffer{ new char[8 * 1024 * 1024] };
  output_buffer[0] = '\0';

  solver->solve(input_buffer.get(), output_buffer.get());

  fputs(output_buffer.get(), output_file.get());
  return 0;
}

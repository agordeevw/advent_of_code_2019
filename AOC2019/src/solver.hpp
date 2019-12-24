#pragma once
#include <memory>

class base_solver
{
public:
  virtual void solve(const char* input, char* output) = 0;
};

template <int day, int subtask>
class solver : public base_solver
{
public:
  void solve(const char* input, char* output) override;
};

std::unique_ptr<base_solver> create_solver(int day, int subtask);

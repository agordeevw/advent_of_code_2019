#include "solver.hpp"

template <int day>
static inline std::unique_ptr<base_solver> create_solver_for_day(int subtask)
{
  switch (subtask)
  {
    case 1: return std::unique_ptr<base_solver>(new solver<day, 1>());
    case 2: return std::unique_ptr<base_solver>(new solver<day, 2>());
    default: return nullptr;
  }
}

std::unique_ptr<base_solver> create_solver(int day, int subtask)
{
  switch (day)
  {
    case 3: return create_solver_for_day<3>(subtask);
    case 4: return create_solver_for_day<4>(subtask);
    case 5: return create_solver_for_day<5>(subtask);
    case 6: return create_solver_for_day<6>(subtask);
    case 7: return create_solver_for_day<7>(subtask);
    case 8: return create_solver_for_day<8>(subtask);
    case 9: return create_solver_for_day<9>(subtask);
    case 10: return create_solver_for_day<10>(subtask);
    case 11: return create_solver_for_day<11>(subtask);
    case 12: return create_solver_for_day<12>(subtask);
    case 13: return create_solver_for_day<13>(subtask);
    case 14: return create_solver_for_day<14>(subtask);
    case 15: return create_solver_for_day<15>(subtask);
    case 16: return create_solver_for_day<16>(subtask);
    case 17: return create_solver_for_day<17>(subtask);
    case 18: return create_solver_for_day<18>(subtask);
    case 19: return create_solver_for_day<19>(subtask);
    case 20: return create_solver_for_day<20>(subtask);
    case 21: return create_solver_for_day<21>(subtask);
    case 22: return create_solver_for_day<22>(subtask);
    case 23: return create_solver_for_day<23>(subtask);
    case 24: return create_solver_for_day<24>(subtask);
    case 25: return create_solver_for_day<25>(subtask);
    default: return nullptr;
  }
}
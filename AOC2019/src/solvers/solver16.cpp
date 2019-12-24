#include <algorithm>
#include <cassert>
#include <unordered_map>
#include <vector>

#include "solver.hpp"
#include "common/vec2.hpp"

constexpr int DAY = 16;
namespace 
{

std::vector<int64_t> fft(std::vector<int64_t> const& v)
{
  int64_t base_pattern[] = { 0, 1, 0, -1 };
  std::vector<int64_t> ret;
  for (uint64_t p = 0; p < v.size(); p++)
  {
    int64_t value = 0;
    for (uint64_t i = 0; i < v.size(); i++)
    {
      uint64_t pattern_idx = ((i + 1) / (p + 1)) % 4;
      value += v[i] * base_pattern[pattern_idx];
    }
    ret.push_back(std::abs(value) % 10);
  }
  return ret;
}

void fft_2_latter_half(std::vector<uint8_t> const& src, std::vector<uint8_t>& dst)
{
  int64_t prefix_sum = 0;
  for (uint64_t p = dst.size() - 1; p >= dst.size() / 2; p--)
  {
    prefix_sum += src[p];
    dst[p] = prefix_sum % 10;
  }
}

}
void solver<DAY, 1>::solve(const char* input, char* output)
{
  std::vector<int64_t> numbers;
  while (*input)
  {
    numbers.push_back(*input - '0');
    input++;
  }

  for (int i = 0; i < 100; i++)
  {
    numbers = fft(numbers);
  }

  for (int i = 0; i < 8; i++)
  {
    sprintf(output, "%lld ", numbers[i]);
    output += strlen(output);
  }
}

void solver<DAY, 2>::solve(const char* input, char* output)
{
  std::vector<uint8_t> numbers;
  while (*input)
  {
    numbers.push_back(*input - '0');
    input++;
  }

  int64_t message_offset = 0;
  for (int i = 0; i < 7; i++)
  {
    message_offset = (message_offset * 10) + numbers[i];
  }

  std::vector<uint8_t> buffers[2];
  for (int i = 0; i < 10000; i++)
  {
    for (uint8_t num : numbers)
    {
      buffers[0].push_back(num);
    }
  }
  buffers[1].resize(buffers[0].size());

  int cur = 0;
  for (int i = 0; i < 100; i++)
  {
    printf("At iter %d...\n", i);
    fft_2_latter_half(buffers[cur], buffers[1 - cur]);
    cur = 1 - cur;
  }

  for (int i = 0; i < 8; i++)
  {
    sprintf(output, "%d", (int)(buffers[cur][i + message_offset]));
    output += strlen(output);
  }
}

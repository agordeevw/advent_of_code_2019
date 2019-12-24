#include <algorithm>
#include <vector>

#include "solver.hpp"

constexpr int DAY = 8;
namespace 
{

namespace task_1
{
struct image_layer
{
  int num_digits(char digit) const
  {
    int ret = 0;
    for (int i = 0; i < 6; i++)
    {
      for (int j = 0; j < 25; j++)
      {
        if (pixels[i][j] == digit)
        {
          ret++;
        }
      }
    }
    return ret;
  }

  char pixels[6][25];
};

using image = std::vector<image_layer>;

image read_image(const char* s)
{
  image img;
  while (*s != 0)
  {
    image_layer layer;
    sscanf(s, "%150c", layer.pixels);
    img.push_back(layer);
    s += 150;
  }
  return img;
}

} // namespace task_1


namespace task_2
{
struct image_layer
{
  int num_digits(char digit) const
  {
    int ret = 0;
    for (int i = 0; i < 6; i++)
    {
      for (int j = 0; j < 25; j++)
      {
        if (pixels[i][j] == digit)
        {
          ret++;
        }
      }
    }
    return ret;
  }

  char pixels[6][25];
};

using image = std::vector<image_layer>;

image read_image(const char* s)
{
  image img;
  while (*s != 0)
  {
    image_layer layer;
    sscanf(s, "%150c", layer.pixels);
    img.push_back(layer);
    s += 150;
  }
  return img;
}

image_layer flatten_image(image const& img)
{
  image_layer ret;
  for (int r = 0; r < 6; r++)
  {
    for (int c = 0; c < 25; c++)
    {
      for (int l = 0; l < img.size(); l++)
      {
        char pixel_color = img[l].pixels[r][c];
        if (pixel_color == '2')
        {
          continue;
        }
        else
        {
          ret.pixels[r][c] = (pixel_color == '1' ? '*' : ' ');
          break;
        }
      }
    }
  }
  return ret;
}
} // namespace task_2

}

void solver<DAY, 1>::solve(const char* input, char* output)
{
  using namespace task_1;

  image img = read_image(input);
  int best_layer = -1;
  int best_layer_num_zeros = INT_MAX;
  for (int i = 0; i < img.size(); i++)
  {
    int num_zeros = img[i].num_digits('0');
    if (num_zeros < best_layer_num_zeros)
    {
      best_layer = i;
      best_layer_num_zeros = num_zeros;
    }
  }
  int ret = img[best_layer].num_digits('1') * img[best_layer].num_digits('2');
  sprintf(output, "%d", ret);
}

void solver<DAY, 2>::solve(const char* input, char* output)
{
  output[0] = 0;

  using namespace task_2;
  image img = read_image(input);
  image_layer flattened_img = flatten_image(img);
  for (int r = 0; r < 6; r++)
  {
    memcpy(output, flattened_img.pixels[r], 25);
    output += 25;
    *output = '\n';
    output++;
  }
  *output = 0;
}

#pragma once
#include <cstdint>

struct vec2
{
  vec2() : x{0}, y{0} {}
  vec2(int v) : x{v}, y{v} {}
  vec2(int x, int y) : x{x}, y{y} {}

  int x;
  int y;
};

inline static vec2 operator+(vec2 l, vec2 r)
{
  return {l.x + r.x, l.y + r.y};
}

inline static vec2 operator+(int s, vec2 r)
{
  return {s + r.x, s + r.y};
}

inline static vec2 operator-(vec2 l, vec2 r)
{
  return {l.x - r.x, l.y - r.y};
}

inline static vec2 operator-(int s, vec2 r)
{
  return {s - r.x, s - r.y};
}

inline static vec2 operator-(vec2 l, int s)
{
  return {l.x - s, l.x - s};
}

inline static vec2 operator*(int s, vec2 v)
{
  return {s * v.x, s * v.y};
}

inline static vec2 operator*(vec2 v, int s)
{
  return {s * v.x, s * v.y};
}

inline static bool operator==(vec2 l, vec2 r)
{
  return l.x == r.x && l.y == r.y;
}

inline static bool operator!=(vec2 l, vec2 r)
{
  return l.x != r.x || l.y != r.y;
}

struct vec2_hasher
{
  inline uint64_t operator()(vec2 v) const
  {
    return v.x + v.y;
  }
};

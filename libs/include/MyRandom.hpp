#pragma once

#include <random>

class MyRandom
{
public:
  MyRandom()=default;
  virtual ~MyRandom()=default;
  template <typename T> T Uniform(T min , T max)
  {
    std::uniform_real_distribution<T> dist{min , max};
    return dist(m_Generator);
  }

  template <typename T> T Gaus(T mean , T sigma)
  {
    std::normal_distribution<T> dist{mean , sigma};
    return dist(m_Generator);
  }

  void setSeed(const unsigned int&);

protected:
  std::mt19937_64 m_Generator;
};

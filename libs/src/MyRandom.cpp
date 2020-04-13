#include "MyRandom.hpp"

void MyRandom::setSeed(const unsigned int& seed)
{
  m_Generator.seed(seed);
}



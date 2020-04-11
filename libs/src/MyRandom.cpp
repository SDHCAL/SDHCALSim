#include "MyRandom.hpp"

MyRandom* MyRandom::theOneTrueInstance = nullptr ;

MyRandom::~MyRandom()
{
	theOneTrueInstance = nullptr ;
}

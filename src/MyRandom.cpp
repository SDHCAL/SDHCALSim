#include "MyRandom.h"

MyRandom* MyRandom::theOneTrueInstance = nullptr ;

MyRandom::~MyRandom()
{
	theOneTrueInstance = nullptr ;
}

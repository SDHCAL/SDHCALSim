#ifndef MyRandom_h
#define MyRandom_h

#include <stdexcept>
#include <Randomize.hh>
#include <random>

class MyRandom
{
		static MyRandom* theOneTrueInstance ;

	public :

		static MyRandom* Instance()
		{
			if ( !theOneTrueInstance )
				initInstance() ;

			return theOneTrueInstance ;
		}

		template <typename T>
		T Uniform(T min , T max)
		{
			std::uniform_real_distribution<T> dist{min , max} ;
			return dist(generator) ;
		}

		template <typename T>
		T Gaus(T mean , T sigma)
		{
			std::normal_distribution<T> dist{mean , sigma} ;
			return dist(generator) ;
		}

	protected :

		MyRandom(unsigned int seed)
			: generator{seed}
		{
			if (theOneTrueInstance)
				throw std::logic_error("MyRandom already exists") ;

			theOneTrueInstance = this ;

			std::cout << "MyRandom initialized with seed : " << seed << std::endl ;
		}

		static void initInstance()
		{
			new MyRandom( static_cast<unsigned int>(CLHEP::HepRandom::getTheSeed()) ) ;
		}

		virtual ~MyRandom() ;

		std::mt19937_64 generator ;
} ;

#endif //MyRandom_h

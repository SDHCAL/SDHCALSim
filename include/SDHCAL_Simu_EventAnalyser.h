#ifndef SDHCAL_Simu_EventAnalyser_h
#define SDHCAL_Simu_EventAnalyser_h 1

#include "SDHCAL_Util_CaloHitVector.h"

#include <string>
#include <iostream>

//lcio stuff
#include "UTIL/LCTypedVector.h"
#include "EVENT/SimCalorimeterHit.h" 
#include "EVENT/CalorimeterHit.h" 
#include "EVENT/LCGenericObject.h"
#include "EVENT/LCEvent.h"

using namespace lcio ; 


template <class T>
class SDHCAL_Simu_CollectionLoader
{
  LCTypedVector<T> *_vector;
  const char *_colName;
 public:
  LCTypedVector<T>& vector() {return *_vector;} 
 SDHCAL_Simu_CollectionLoader(const char* namecol=NULL)
   : _vector(NULL), _colName(namecol) {}
  ~SDHCAL_Simu_CollectionLoader() {delete _vector;} 

  bool load(LCEvent *event)
  {
    if (_colName==NULL) return false;
    delete _vector;
    _vector=NULL;
    try
      {
	LCCollection* col=event->getCollection(_colName);
	if (NULL == col) return false;
	_vector=new LCTypedVector<T>(col);
      }
    catch( lcio::DataNotAvailableException& e )
      {
	std::cerr << e.what() << std::endl ;
	return false;
      }
    return true;
  }
  
};

class SDHCAL_Simu_EventAnalyser
{
 public:
  SDHCAL_Simu_EventAnalyser(const char* simHits=NULL, const char* recoHits=NULL,  const char* steps=NULL);
  
  void newEvent(LCEvent *event);
  void printStat();
    
 private:
  
  SDHCAL_Simu_CollectionLoader<CalorimeterHit> _recoHits;
  SDHCAL_Simu_CollectionLoader<SimCalorimeterHit> _simHits;
  SDHCAL_Simu_CollectionLoader<LCGenericObject> _steps;
 
  bool _OKreco,_OKsim,_OKsteps;
  
  //compteurs
  int _totalEvents;
  
};

#endif

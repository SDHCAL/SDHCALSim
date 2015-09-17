#ifndef SDHCAL_Simu_EventAnalyser_h
#define SDHCAL_Simu_EventAnalyser_h 1

#include "SDHCAL_Util_CaloHitVector.h"

#include <string>
#include <iostream>
#include <map>

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

/*class HitDataStatByPlan
{
 public:
  std::map<unsigned int,int> distributionNombreHit;
  std::vector<double> hitDistance;
  std::vector<double> hitDistanceMin;
};
*/

template <class HIT>
class HitDataStat
{
  std::map<unsigned int,int> distributionNombreHit;
  std::map<unsigned int,int> distributionNombrePlan;
  std::map<unsigned int,int> distributionNumeroPlan;
 public:
  HitDataStat() {}

  void Add(LCTypedVector<HIT>& hits)
    {
      distributionNombreHit[hits.size()]++;
      if (hits.empty()) return;
      SDHCAL::LCIO_hitVectorManipulation<HIT> a;
      typedef typename SDHCAL::LCIO_hitVectorManipulation<HIT>::TCaloHitPairIterator LocalCaloHitPairIterator;
      //std::vector< LocalCaloHitPairIterator > b=a.partition_byLayer(hits);
      std::vector< LocalCaloHitPairIterator > b=a.partition_byLayer(hits);
      distributionNombrePlan[b.size()]++;
      for (typename std::vector< LocalCaloHitPairIterator >::iterator it=b.begin(); it!= b.end(); ++it)
	{
	  const HIT* h=*(it->first);
	  SDHCAL::LCIO_hitVectorManipulation<HIT>::CalorimeterHit_lessCellID::m_decoder.setValue(h->getCellID0(),h->getCellID1());
	  int layer= SDHCAL::LCIO_hitVectorManipulation<HIT>::CalorimeterHit_lessCellID::m_decoder.BF()["K-1"];
	  distributionNumeroPlan[layer]++;
	}
    }


  void showMap(std::map<unsigned int,int> &m, std::ostream& flux=std::cout, char ending='\n')
    {
      for(std::map<unsigned int,int>::const_iterator it = m.begin(); it != m.end(); ++it)
	flux << it->first << ":" << it->second << ",  ";
      flux << ending;
  }

  void printDistr()
  {
    std::cout << "Number of hit distribution : "; showMap(distributionNombreHit);
    std::cout << "Number of plane distribution : "; showMap(distributionNombrePlan);
    std::cout << "Numero of plane distribution : "; showMap(distributionNumeroPlan);
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
  
  HitDataStat<CalorimeterHit> _recoHitsStat;
  HitDataStat<SimCalorimeterHit> _simHitsStat;
  

  bool _OKreco,_OKsim,_OKsteps;
  
  //compteurs
  int _totalEvents;
  
};

#endif

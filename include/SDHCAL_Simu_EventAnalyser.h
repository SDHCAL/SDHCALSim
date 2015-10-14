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
  unsigned int _maxWarning;
  unsigned int _warningCount;
 public:
  LCTypedVector<T>& vector() {return *_vector;} 
 SDHCAL_Simu_CollectionLoader(const char* namecol=NULL,unsigned int max=10)
   : _vector(NULL), _colName(namecol), _maxWarning(max), _warningCount(0) {}
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
	if (_warningCount<_maxWarning)
	  {
	    ++_warningCount;
	    std::cerr << e.what() << "  Warning " << _warningCount<<"/" << _maxWarning << std::endl ;
	  }
	return false;
      }
    return true;
  }
  
};

template <class HIT>
class HitDataStatByPlan
{
 public:
  std::map<unsigned int,int> distributionNombreHit;
  std::vector<double> hitDistance;
  std::vector<double> hitDistanceMin;

  HitDataStatByPlan() {}
  typedef typename SDHCAL::LCIO_hitVectorManipulation<HIT>::TCaloHitPairIterator LocalCaloHitPairIterator;
  void FillNhit(LocalCaloHitPairIterator& p) { distributionNombreHit[std::distance(p.first,p.second)]++; }
};


template <class HIT>
class HitDataStat
{
  std::map<unsigned int,int> distributionNombreHit;
  std::map<unsigned int,int> distributionNombrePlan;
  std::map<unsigned int,int> distributionNumeroPlan;
  std::map<unsigned int, HitDataStatByPlan<HIT> > statByPlan;
 public:
  HitDataStat() {}

  void Add(LCTypedVector<HIT>& hits)
    {
      distributionNombreHit[hits.size()]++;
      if (hits.empty()) return;
      SDHCAL::LCIO_hitVectorManipulation<HIT> a;
      typedef typename SDHCAL::LCIO_hitVectorManipulation<HIT>::TCaloHitPairIterator LocalCaloHitPairIterator;
      std::vector< LocalCaloHitPairIterator > b=a.partition_byLayer(hits);
      distributionNombrePlan[b.size()]++;
      for (typename std::vector< LocalCaloHitPairIterator >::iterator it=b.begin(); it!= b.end(); ++it)
	{
	  const HIT* h=*(it->first);
	  SDHCAL::LCIO_hitVectorManipulation<HIT>::CalorimeterHit_lessCellID::m_decoder.setValue(h->getCellID0(),h->getCellID1());
	  int layer= SDHCAL::LCIO_hitVectorManipulation<HIT>::CalorimeterHit_lessCellID::m_decoder.BF()["K-1"];
	  distributionNumeroPlan[layer]++;
	  statByPlan[layer].FillNhit(*it);
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
    const char *spaces="     ";
    for (typename std::map<unsigned int, HitDataStatByPlan<HIT> >::iterator it=statByPlan.begin(); it != statByPlan.end(); ++it)
      {
	std::cout << "Subreport for layer : " << it->first << " : " << std::endl;
	std::cout << spaces << "Number of hit in plane distribution : "; showMap(it->second.distributionNombreHit);
      }
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

#ifndef SDHCAL_Simu_EventAnalyser_h
#define SDHCAL_Simu_EventAnalyser_h 1

#include "SDHCAL_Util_CaloHitVector.h"

#include <string>
#include <iostream>
#include <map>
#include <math.h>

//lcio stuff
#include "UTIL/LCTypedVector.h"
#include "EVENT/SimCalorimeterHit.h" 
#include "EVENT/CalorimeterHit.h" 
#include "EVENT/LCGenericObject.h"
#include "EVENT/LCEvent.h"

#include "NNClusters.h"  // in MarlinUtil

//ROOT stuff
#include "TDirectory.h"
#include "TString.h"

using namespace lcio ; 

//////////////////////////////////////////////////////////////////////////////
//
//  Helper function to make ROOT histos
//
//////////////////////////////////////////////////////////////////////////////
void SDHCALmakeHisto(const char *name, const char* title,std::map<unsigned int,int>& data,unsigned int minNumberOfBin=0,unsigned int maxNumberOfBin=0);
void SDHCALmakeHisto(const char *name, const char* title,std::map<float,int>& data,unsigned int NBin=300,float maxValue=0);


//////////////////////////////////////////////////////////////////////////////
//
//  Class to load a LCIO collection 
//  template parameter should be a LCIO object
//
//////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////
//
//  distance as a function of I and J for clusterization
//
//////////////////////////////////////////////////////////////////////////////
template <class HIT>
class SDHCAL_IJDistance
{
 public:
  /** Required typedef for cluster algorithm */
  typedef HIT hit_type ;

  /** C'tor takes merge distance */
  SDHCAL_IJDistance(float dCut) : _dCutSquared( dCut*dCut ) {}
  
  /** Merge condition: true if distance  is less than dCut given in the C'tor.*/ 
  inline bool mergeHits( GenericHit<HIT>* h0, GenericHit<HIT>* h1)
  {
    SDHCAL::LCIO_hitVectorManipulation<HIT>::CalorimeterHit_lessCellID::m_decoder.setValue(h0->first->getCellID0(),h0->first->getCellID1());
    int I0=SDHCAL::LCIO_hitVectorManipulation<HIT>::CalorimeterHit_lessCellID::m_decoder.BF()["I"];
    int J0=SDHCAL::LCIO_hitVectorManipulation<HIT>::CalorimeterHit_lessCellID::m_decoder.BF()["J"];
    int layer0=SDHCAL::LCIO_hitVectorManipulation<HIT>::CalorimeterHit_lessCellID::m_decoder.BF()["K-1"];
    SDHCAL::LCIO_hitVectorManipulation<HIT>::CalorimeterHit_lessCellID::m_decoder.setValue(h1->first->getCellID0(),h1->first->getCellID1());
    int I1=SDHCAL::LCIO_hitVectorManipulation<HIT>::CalorimeterHit_lessCellID::m_decoder.BF()["I"];
    int J1=SDHCAL::LCIO_hitVectorManipulation<HIT>::CalorimeterHit_lessCellID::m_decoder.BF()["J"];
    int layer1=SDHCAL::LCIO_hitVectorManipulation<HIT>::CalorimeterHit_lessCellID::m_decoder.BF()["K-1"];
    if (layer0 != layer1) return false;
    return (I1-I0)*(I1-I0) + (J1-J0)*(J1-J0) < _dCutSquared;
  }
  
 private:
  float _dCutSquared ;
};


//////////////////////////////////////////////////////////////////////////////
//
//  functions to clusterize
//
//////////////////////////////////////////////////////////////////////////////

template <class HIT,class PRED>
  GenericClusterVec<HIT>* SDHCALclusterize(typename SDHCAL::LCIO_hitVectorManipulation<HIT>::TCaloHitPairIterator p, PRED* Pred)
{
  std::vector< GenericHit<HIT>* > v;
  for (typename SDHCAL::LCIO_hitVectorManipulation<HIT>::TCaloHitIterator it=p.first; it!=p.second; ++it)
    v.push_back( new GenericHit<HIT>(*it) );
  GenericClusterVec<HIT>* ptocl=new GenericClusterVec<HIT>();
  GenericClusterVec<HIT>& cl=*ptocl ;

  cluster( v.begin() , v.end() , std::back_inserter( cl )  , Pred ) ;
  //isolated hits don't make it into a cluster :
  for (typename std::vector< GenericHit<HIT>* >::iterator ith=v.begin(); ith!= v.end(); ++ith)
    if (NULL==(*ith)->second)
      {
	GenericCluster<HIT>* acluster=new GenericCluster<HIT>(*ith);
	cl.push_back(acluster);
      }

  return ptocl;
}

//clustering with cartesian distance : will work for CalorimeterHit and SimCalorimeterHit
template <class HIT>
  GenericClusterVec<HIT>* SDHCALclusterize_cartesian(typename SDHCAL::LCIO_hitVectorManipulation<HIT>::TCaloHitPairIterator p, float dcut)
{
  NNDistance< HIT, float> max_distance_for_adjacent_hits_mm(dcut);
  return SDHCALclusterize<HIT,NNDistance< HIT, float> >(p,&max_distance_for_adjacent_hits_mm);
}

//clustering in units of cell size (uses I and J in CellID
template <class HIT>
  GenericClusterVec<HIT>* SDHCALclusterize_IJcartesian(typename SDHCAL::LCIO_hitVectorManipulation<HIT>::TCaloHitPairIterator p, float dcut)
{
  SDHCAL_IJDistance<HIT> max_distance_for_adjacent_hits_IJ(dcut);
  return SDHCALclusterize<HIT,SDHCAL_IJDistance<HIT> >(p,&max_distance_for_adjacent_hits_IJ);
}


//////////////////////////////////////////////////////////////////////////////
//
//  Class to compute hit distribution in a RPC plane
//
//////////////////////////////////////////////////////////////////////////////

template <class HIT>
class HitDataStatByPlan
{
  float carre(float x) {return x*x;}
  float somme3carres(const float *v1,const float *v2) {return carre(v1[0]-v2[0])+ carre(v1[1]-v2[1])+ carre(v1[2]-v2[2]);}
  float round3decimal(float x) {return int(1000*x)/1000.0;}
 public:
  std::map<unsigned int,int> distributionNombreHit;
  std::map<float,int> hitDistanceIJ;
  std::map<unsigned int,int> distributionNombreClusterSide;
  std::map<unsigned int,int> distributionSizeClusterSide;
  std::map<unsigned int,int> distributionNombreClusterSideCorner;
  std::map<unsigned int,int> distributionSizeClusterSideCorner;

  HitDataStatByPlan() {}
  typedef typename SDHCAL::LCIO_hitVectorManipulation<HIT>::TCaloHitPairIterator LocalCaloHitPairIterator;
  void FillNhit(LocalCaloHitPairIterator& p) { distributionNombreHit[std::distance(p.first,p.second)]++; }
  void FillDistanceInfo(LocalCaloHitPairIterator &p)  
  {
    typedef typename SDHCAL::LCIO_hitVectorManipulation<HIT>::TCaloHitVector::iterator LocalCaloHitIterator;
    std::vector<std::pair<int,int> > posIJ;
    
    for (LocalCaloHitIterator it=p.first; it != p.second ; ++it) 
      {
	const HIT* h=*it;
	SDHCAL::LCIO_hitVectorManipulation<HIT>::CalorimeterHit_lessCellID::m_decoder.setValue(h->getCellID0(),h->getCellID1());
	posIJ.push_back(std::pair<int,int>(SDHCAL::LCIO_hitVectorManipulation<HIT>::CalorimeterHit_lessCellID::m_decoder.BF()["I"],
					   SDHCAL::LCIO_hitVectorManipulation<HIT>::CalorimeterHit_lessCellID::m_decoder.BF()["J"]));
      }
    for (unsigned int i1=0; i1<posIJ.size(); ++i1)
      {
	for (unsigned int i2=i1+1; i2 < posIJ.size(); ++i2)
	  {
	    hitDistanceIJ[sqrt(carre(posIJ[i1].first-posIJ[i2].first)+carre(posIJ[i1].second-posIJ[i2].second))]++;
	  }
      }

    //clusterisation
    GenericClusterVec<HIT>* ClusterSide=SDHCALclusterize_IJcartesian<HIT>(p,1.1);
    distributionNombreClusterSide[ClusterSide->size()]++;
    for (typename GenericClusterVec<HIT>::iterator itcl=ClusterSide->begin(); itcl != ClusterSide->end(); ++itcl)
      distributionSizeClusterSide[(*itcl)->size()]++;

    GenericClusterVec<HIT>* ClusterSideCorner=SDHCALclusterize_IJcartesian<HIT>(p,1.6);
    distributionNombreClusterSideCorner[ClusterSideCorner->size()]++;
    for (typename GenericClusterVec<HIT>::iterator itcl=ClusterSideCorner->begin(); itcl != ClusterSideCorner->end(); ++itcl)
      distributionSizeClusterSideCorner[(*itcl)->size()]++;
  }

  void saveInHisto()
  {
    SDHCALmakeHisto("Nhit","Number of hits",distributionNombreHit);
    SDHCALmakeHisto("hitDistance","Distance between hits in plane in cell size units",hitDistanceIJ);
    SDHCALmakeHisto("NClusterSide","Number of NN clusters : distance cut at 1.1 cell size",distributionNombreClusterSide);
    SDHCALmakeHisto("SizeClusterSide","NN clusters size : distance cut at 1.1 cell size",distributionSizeClusterSide);
    SDHCALmakeHisto("NClusterSideCorner","Number of NN clusters : distance cut at 1.1 cell size",distributionNombreClusterSideCorner);
    SDHCALmakeHisto("SizeClusterSideCorner","NN clusters size : distance cut at 1.6 cell size",distributionSizeClusterSideCorner);    
  }
};




//////////////////////////////////////////////////////////////////////////////
//
//  Class to compute hit distribution in many RPC planes
//
//////////////////////////////////////////////////////////////////////////////

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
	  statByPlan[layer].FillDistanceInfo(*it);
	  
	}
    }

  template <class T>
  void showMap(std::map<T,int> &m, std::ostream& flux=std::cout, char ending='\n')
    {
      for(typename std::map<T,int>::const_iterator it = m.begin(); it != m.end(); ++it)
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
	std::cout << spaces << "Number of hits in plane distribution : "; showMap(it->second.distributionNombreHit);
	std::cout << spaces << "Distance between hits in plane distribution : "; showMap(it->second.hitDistanceIJ);
	std::cout << spaces << "Number of clusters in plane side clustering : "; showMap(it->second.distributionNombreClusterSide);
	std::cout << spaces << "Number of clusters in plane side and corner clustering : "; showMap(it->second.distributionNombreClusterSideCorner);
	std::cout << spaces << "Size of clusters in plane side clustering : "; showMap(it->second.distributionSizeClusterSide);
	std::cout << spaces << "Size of clusters in plane side and corner clustering : "; showMap(it->second.distributionSizeClusterSideCorner);
      }
  }

  void saveInHisto()
  {
    SDHCALmakeHisto("Nhit","Total number of hits",distributionNombreHit);
    SDHCALmakeHisto("NPlan","Number of layers with hits",distributionNombrePlan);
    SDHCALmakeHisto("NumeroPlan","Layer with hits",distributionNumeroPlan);
    TDirectory *dir=gDirectory;
    for (typename std::map<unsigned int, HitDataStatByPlan<HIT> >::iterator it=statByPlan.begin(); it != statByPlan.end(); ++it)
      {
	TString a("Plan"); a+=it->first;
	dir->mkdir(a);
	dir->cd(a);
	it->second.saveInHisto();
      }
  }
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class to analyze simulated/reconstructed GRPC hits
//
//////////////////////////////////////////////////////////////////////////////

class SDHCAL_Simu_EventAnalyser
{
 public:
  SDHCAL_Simu_EventAnalyser(const char* simHits=NULL, const char* recoHits=NULL,  const char* steps=NULL);
  
  void newEvent(LCEvent *event);
  void printStat();
  void writeROOTfile(const char* filename);
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

#ifndef SDHCAL_Util_CaloHitVector_h
#define SDHCAL_Util_CaloHitVector_h 1

#include "lcio.h"
#include <string>
#include <vector>
#include <set>
#include <algorithm>

//lcio stuff
#include "UTIL/LCTypedVector.h"
#include "EVENT/CalorimeterHit.h"
#include "EVENT/SimCalorimeterHit.h"
#include "EVENT/RawCalorimeterHit.h"
#include <UTIL/CellIDDecoder.h>

using namespace lcio ;



namespace SDHCAL 
{

  //////////////////////////////////////////////////////////////////////////////////////////
  // 
  //  typedef for various kind of vectors of calorimeter hit and iterators
  //
  //  LCTypedVector is template <class T> class LCTypedVector : public  std::vector<T*> 
  //  with following constructor LCTypedVector( EVENT::LCCollection* col )
  //  and with accessor : EVENT::LCCollection* col();
  //
  //////////////////////////////////////////////////////////////////////////////////////////
  
  typedef LCTypedVector<CalorimeterHit> CaloHitVector;      //type = vector<CalorimeterHit*>
  typedef CaloHitVector::iterator CaloHitIterator;          //type = CalorimeterHit**
  typedef std::pair< CaloHitIterator, CaloHitIterator > CaloHitPairIterator;

  typedef LCTypedVector<SimCalorimeterHit> SimCaloHitVector;      //type = vector<SimCalorimeterHit*>
  typedef SimCaloHitVector::iterator SimCaloHitIterator;          //type = SimCalorimeterHit**
  typedef std::pair< SimCaloHitIterator, SimCaloHitIterator > SimCaloHitPairIterator;

  typedef LCTypedVector<RawCalorimeterHit> RawCaloHitVector;      //type = vector<RawCalorimeterHit*>
  typedef RawCaloHitVector::iterator RawCaloHitIterator;          //type = RawCalorimeterHit**
  typedef std::pair< RawCaloHitIterator, RawCaloHitIterator > RawCaloHitPairIterator;

  //////////////////////////////////////////////////////////////////////////////////////////
  // 
  //  exception for when trying to access LCIO (XXX)CalorimeterHit parameters that are not set
  //
  //////////////////////////////////////////////////////////////////////////////////////////

  class CalorimeterHitPositionNotSet_Exception : public std::exception
  {
  public:
    virtual const char* what() const throw() 
    {
      return "CalorimeterHitPositionNotSet_Exception :: Attenpt to use LCIO::CalorimeterHit getPosition while data is not available";
    }
  };
  


  //////////////////////////////////////////////////////////////////////////////////////////
  // 
  //  functions and functors to be used in algorithms below (sorting, partitioning)
  //
  //////////////////////////////////////////////////////////////////////////////////////////

  //helper functions
  inline float dotProduct(const float *a, const float* b) { return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];}



  // compare two pointers to calorimeter hits based on the Z position of the pointed hits
  // valid for CalorimeterHit and SimCalorimeterHit
  template <class HIT>
    inline bool TempCalorimeterHit_lessZ(const HIT* left , const HIT* right)
    {
      if ( 0 == left->getPosition() || 0 == right->getPosition() ) return true;
      return left->getPosition()[2]<right->getPosition()[2];
    } 
  //rename function for code clarity 
  inline bool CalorimeterHit_lessZ(const CalorimeterHit* left , const CalorimeterHit* right) {return TempCalorimeterHit_lessZ<CalorimeterHit>(left,right);}
  inline bool SimCalorimeterHit_lessZ(const SimCalorimeterHit* left , const SimCalorimeterHit* right) {return TempCalorimeterHit_lessZ<SimCalorimeterHit>(left,right);}

  


  //compare two pointers to calorimeter hits based on the position in a given direction
  // valid for CalorimeterHit and SimCalorimeterHit
  template <class HIT>
    class TempCalorimeterHit_lessDir
    {
    public:
      TempCalorimeterHit_lessDir(float x, float y, float z) 
	{m_direction[0]=x; m_direction[1]=y; m_direction[2]=z;}
      TempCalorimeterHit_lessDir(float *dir)
	{m_direction[0]=dir[0]; m_direction[1]=dir[1]; m_direction[2]=dir[2];}
      bool operator()(const HIT* left , const HIT* right) {return getVal(left)<getVal(right);}
    private:
      float m_direction[3];
      float getVal(const HIT* hit) { if ( 0 == hit->getPosition() ) return 0; else return dotProduct(hit->getPosition(),m_direction);}
    };
  //typedef for code clarity
  typedef TempCalorimeterHit_lessDir<CalorimeterHit> CalorimeterHit_lessDir;
  typedef TempCalorimeterHit_lessDir<SimCalorimeterHit> SimCalorimeterHit_lessDir;
  
  
  //compare two pointers to calorimeter hits by radius
  // valid for CalorimeterHit and SimCalorimeterHit
  template <class HIT>
    inline bool TempCalorimeterHit_lessRadius(const HIT* left , const HIT* right)
    {
      if ( 0 == left->getPosition() || 0 == right->getPosition() ) return true;
      return dotProduct(left->getPosition(),left->getPosition()) < dotProduct(right->getPosition(),right->getPosition());
    }

  //rename function for code clarity 
  inline bool CalorimeterHit_lessRadius(const CalorimeterHit* left , const CalorimeterHit* right) {return TempCalorimeterHit_lessRadius(left,right);}
  inline bool SimCalorimeterHit_lessRadius(const SimCalorimeterHit* left , const SimCalorimeterHit* right) {return TempCalorimeterHit_lessRadius(left,right);}
  
  
  //compare two pointers to calorimeter hits based on value in the cell id
  //valid for all calorimeter hit types
  template <class HIT>
    class TempCalorimeterHit_lessCellID 
    {
    public:
    TempCalorimeterHit_lessCellID(const std::string& encoder_str, const std::string& coding) 
      : m_decoder(encoder_str), m_codingString(coding) {}
    TempCalorimeterHit_lessCellID( const LCCollection* col, const std::string& coding) 
      : m_decoder(col), m_codingString(coding) {}
      bool operator()(const HIT* left , const HIT* right) 
      {return m_decoder(left)[m_codingString]<m_decoder(right)[m_codingString];}
    private:
      CellIDDecoder<HIT> m_decoder;
      std::string m_codingString;
    };
  //typedef for code clarity
  typedef TempCalorimeterHit_lessCellID<CalorimeterHit> CalorimeterHit_lessCellID;
  typedef TempCalorimeterHit_lessCellID<SimCalorimeterHit> SimCalorimeterHit_lessCellID;
  typedef TempCalorimeterHit_lessCellID<RawCalorimeterHit> RawCalorimeterHit_lessCellID;

  


  //////////////////////////////////////////////////////////////////////////////////////////
  // 
  //  sorting functions
  //
  //////////////////////////////////////////////////////////////////////////////////////////

  template <class HIT>
    inline void TempCalorimeterHitVector_sortbyZ(LCTypedVector<HIT>& v)
    {
      std::sort(v.begin(),v.end(),TempCalorimeterHit_lessZ<HIT>);
    }
  inline void CalorimeterHitVector_sortbyZ(CaloHitVector& v) {return TempCalorimeterHitVector_sortbyZ<CalorimeterHit>(v);}
  inline void SimCalorimeterHitVector_sortbyZ(SimCaloHitVector& v) {return TempCalorimeterHitVector_sortbyZ<SimCalorimeterHit>(v);}


  template <class HIT>
    inline void TempCalorimeterHitVector_sortbyPosInDir(LCTypedVector<HIT>& v,float *dir)
    {
      std::sort(v.begin(),v.end(),TempCalorimeterHit_lessDir<HIT>(dir));
    }
  inline void CalorimeterHitVector_sortbyPosInDir(CaloHitVector& v,float *dir) {return TempCalorimeterHitVector_sortbyPosInDir(v,dir);}
  inline void SimCalorimeterHitVector_sortbyPosInDir(SimCaloHitVector& v,float *dir) {return TempCalorimeterHitVector_sortbyPosInDir(v,dir);}


template <class HIT>
  inline void TempCalorimeterHitVector_sortbyRadius(LCTypedVector<HIT>& v)
    {
      std::sort(v.begin(),v.end(),TempCalorimeterHit_lessRadius<HIT>);
    }
 inline void CalorimeterHitVector_sortbyRadius(CaloHitVector& v) {return TempCalorimeterHitVector_sortbyRadius<CalorimeterHit>(v);}
 inline void SimCalorimeterHitVector_sortbyRadius(SimCaloHitVector& v) {return TempCalorimeterHitVector_sortbyRadius<SimCalorimeterHit>(v);}

 template <class HIT>
   inline void TempCalorimeterHitVector_sortbyLayer(LCTypedVector<HIT>& v)
   {
     std::sort(v.begin(),v.end(), TempCalorimeterHit_lessCellID<HIT>(v.col(),"K-1") );
   }
 inline void CalorimeterHitVector_sortbyLayer(CaloHitVector& v) {return TempCalorimeterHitVector_sortbyLayer<CalorimeterHit>(v);}
 inline void SimCalorimeterHitVector_sortbyLayer(SimCaloHitVector& v) {return TempCalorimeterHitVector_sortbyLayer<SimCalorimeterHit>(v);}
 inline void RawCalorimeterHitVector_sortbyLayer(RawCaloHitVector& v) {return TempCalorimeterHitVector_sortbyLayer<RawCalorimeterHit>(v);}



  //////////////////////////////////////////////////////////////////////////////////////////
  // 
  //  partition a sorted vector by some values
  //  the partition returns pairs of iterators (pointers) 
  //  with each pairs of iterator being beginning and end of the 'equal_range' sequance
  //
  //  exemple, if you assume the hits are sorted by layer number
  //  and that the vector of hit layer is 
  //  values    ==   1 1 1 1 2 2 4 5 5
  //  iterators ==   A B C D E F G H I J
  //  then the partition function will return a vector of 4 CaloHitPairIterator
  //  with element[0]=<A,E>, element[1]=<E,G>, element[2]=<G,H>, element[3]=<H,J> 
  //
  //////////////////////////////////////////////////////////////////////////////////////////

  //helper function : return the range of hits within [first,last[ with the same 'value' as the first hit
  //first,last are iterator on a sorted vector
  template <class Pred>
    inline CaloHitPairIterator equal_range(CaloHitIterator  first, CaloHitIterator  last, Pred &p){return std::equal_range(first, last, *first ,p);}
  template <class Pred>
    inline SimCaloHitPairIterator equal_range(SimCaloHitIterator  first, SimCaloHitIterator  last, Pred &p){return std::equal_range(first, last, *first ,p);}
  template <class Pred>
    inline RawCaloHitPairIterator equal_range(RawCaloHitIterator  first, RawCaloHitIterator  last, Pred &p){return std::equal_range(first, last, *first ,p);}


  //the main partition function
  template <class Pred,class HIT>
    class Tempcalohitvec_partition
  {
  public:
    typedef LCTypedVector<HIT> HitVector;
   typedef typename LCTypedVector<HIT>::iterator THitIterator;
    typedef std::pair< THitIterator, THitIterator> THitPairIterator;
     std::vector< THitPairIterator > 
      calohitvec_partition(THitIterator first, THitIterator last, Pred &p)
      {
	std::vector< THitPairIterator >  res;
	while (first != last )
	{
	  THitPairIterator a=equal_range<Pred>(first,last,p);
	  res.push_back( a );
	  first=a.second;
	}
	return res;
      }
  };
  template <class Pred>
    inline std::vector< CaloHitPairIterator > calohitvec_partition(CaloHitIterator  first, CaloHitIterator  last, Pred &p)
    {
      Tempcalohitvec_partition<Pred,CalorimeterHit> a;
      return a.calohitvec_partition(first,last,p);
    }
  template <class Pred>
    inline std::vector< SimCaloHitPairIterator > calohitvec_partition(SimCaloHitIterator  first, SimCaloHitIterator  last, Pred &p)
    {
      Tempcalohitvec_partition<Pred,SimCalorimeterHit> a;
      return a.calohitvec_partition(first,last,p);
    }
  template <class Pred>
    inline std::vector< RawCaloHitPairIterator > calohitvec_partition(RawCaloHitIterator  first, RawCaloHitIterator  last, Pred &p)
    {
      Tempcalohitvec_partition<Pred,RawCalorimeterHit> a;
      return a.calohitvec_partition(first,last,p);
    }
  
  
  //partition against Z position
  inline std::vector< CaloHitPairIterator > partition_byZ(CaloHitVector &v)
    { 
      CalorimeterHitVector_sortbyZ(v);
      return calohitvec_partition(v.begin(), v.end(), CalorimeterHit_lessZ ); 
    }
  inline std::vector< SimCaloHitPairIterator > partition_byZ(SimCaloHitVector &v)
  { 
    SimCalorimeterHitVector_sortbyZ(v);
    return calohitvec_partition(v.begin(), v.end(), SimCalorimeterHit_lessZ ); 
  }
  
  //partition against position along dir 
  inline std::vector< CaloHitPairIterator > partition_byPosInDir(CaloHitVector& v,float *dir)
    {
      CalorimeterHit_lessDir predicate(dir);
      std::sort(v.begin(),v.end(),predicate);
      return calohitvec_partition(v.begin(), v.end(), predicate );
    }

  //partition against layer number
  inline std::vector< CaloHitPairIterator > partition_byLayer(CaloHitVector& v)
    {
      CalorimeterHit_lessCellID predicate(v.col(),"K-1");
      std::sort(v.begin(),v.end(), predicate );
      return calohitvec_partition(v.begin(), v.end(), predicate );
    }

  
}

#endif

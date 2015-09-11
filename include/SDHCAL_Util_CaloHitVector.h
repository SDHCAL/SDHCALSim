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
  inline bool CalorimeterHit_lessZ(const CalorimeterHit* left , const CalorimeterHit* right)
    {
      if ( 0 == left->getPosition() || 0 == right->getPosition() ) return true;
      return left->getPosition()[2]<right->getPosition()[2];
    } 

  //compare two pointers to calorimeter hits based on the position in a given direction
  class CalorimeterHit_lessDir
  {
  public:
    CalorimeterHit_lessDir(float x, float y, float z) 
      {m_direction[0]=x; m_direction[1]=y; m_direction[2]=z;}
    CalorimeterHit_lessDir(float *dir)
      {m_direction[0]=dir[0]; m_direction[1]=dir[1]; m_direction[2]=dir[2];}
    bool operator()(const CalorimeterHit* left , const CalorimeterHit* right) {return getVal(left)<getVal(right);}
  private:
    float m_direction[3];
    float getVal(const CalorimeterHit* hit) { if ( 0 == hit->getPosition() ) return 0; else return dotProduct(hit->getPosition(),m_direction);}
  };

  //compare two pointers to calorimeter hits by radius
  inline bool CalorimeterHit_lessRadius(const CalorimeterHit* left , const CalorimeterHit* right)
    {
      if ( 0 == left->getPosition() || 0 == right->getPosition() ) return true;
      return dotProduct(left->getPosition(),left->getPosition()) < dotProduct(right->getPosition(),right->getPosition());
    }

  //compare two pointers to calorimeter hits based on value in the cell id
  class CalorimeterHit_lessCellID 
  {
  public:
    CalorimeterHit_lessCellID(const std::string& encoder_str, const std::string& coding) 
      : m_decoder(encoder_str), m_codingString(coding) {}
    CalorimeterHit_lessCellID( const LCCollection* col, const std::string& coding) 
      : m_decoder(col), m_codingString(coding) {}
    bool operator()(const CalorimeterHit* left , const CalorimeterHit* right) 
    {return m_decoder(left)[m_codingString]<m_decoder(right)[m_codingString];}
  private:
    CellIDDecoder<CalorimeterHit> m_decoder;
    std::string m_codingString;
  };



  //////////////////////////////////////////////////////////////////////////////////////////
  // 
  //  sorting functions
  //
  //////////////////////////////////////////////////////////////////////////////////////////

  inline void CalorimeterHitVector_sortbyZ(CaloHitVector& v)
    {
      std::sort(v.begin(),v.end(),CalorimeterHit_lessZ);
    }

  inline void CalorimeterHitVector_sortbyPosInDir(CaloHitVector& v,float *dir)
    {
      std::sort(v.begin(),v.end(),CalorimeterHit_lessDir(dir));
    }

  inline void CalorimeterHitVector_sortbyRadius(CaloHitVector& v)
    {
      std::sort(v.begin(),v.end(),CalorimeterHit_lessRadius);
    }

  inline void CalorimeterHitVector_sortbyLayer(CaloHitVector& v)
    {
      std::sort(v.begin(),v.end(), CalorimeterHit_lessCellID(v.col(),"K-1") );
    }



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
  //  then the partition function will return a vector of 4 caloHitPairIterator
  //  with element[0]=<A,E>, element[1]=<E,G>, element[2]=<G,H>, element[3]=<H,J> 
  //
  //////////////////////////////////////////////////////////////////////////////////////////

  //helper function : return the range of hits within [first,last[ with the same 'value' as the first hit
  //first,last are iterator on a sorted vector
  template <class Pred>
    inline CaloHitPairIterator equal_range(CaloHitIterator  first, CaloHitIterator  last, Pred &p)
    {
      return std::equal_range(first, last, *first ,p);
    }


  //the main partition function
  template <class Pred>
    std::vector< CaloHitPairIterator > calohitvec_partition(CaloHitIterator  first, CaloHitIterator  last, Pred &p)
    {
      std::vector< CaloHitPairIterator > res;
      while (first != last )
	{
	  CaloHitPairIterator a=equal_range<Pred>(first,last,p);
	  res.push_back( a );
	  first=a.second;
	}
      return res;
    }
  

  //partition against Z position
  inline std::vector< CaloHitPairIterator > partition_byZ(CaloHitVector &v)
    { 
      CalorimeterHitVector_sortbyZ(v);
      return calohitvec_partition(v.begin(), v.end(), CalorimeterHit_lessZ ); 
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

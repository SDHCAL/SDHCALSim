#ifndef SDHCAL_Util_CaloHitVector_template_h
#define SDHCAL_Util_CaloHitVector_template_h 1

#include "lcio.h"
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <iostream>

//lcio stuff
#include "UTIL/LCTypedVector.h"
//#include "EVENT/CalorimeterHit.h"
//#include "EVENT/SimCalorimeterHit.h"
//#include "EVENT/RawCalorimeterHit.h"
#include <UTIL/CellIDDecoder.h>

using namespace lcio ;



namespace SDHCAL 
{
  //kind of static BitField64
  class MyCellIDDecoder
  {
  public:
  MyCellIDDecoder() : _bf(NULL), _col(NULL) {}
  ~MyCellIDDecoder() {delete _bf;}

  inline void setCollection(std::string tut)
  {
    if (tut.size()==0) tut="DUMMY:8";
    delete _bf;
    _bf=new BitField64(tut);
  }

  inline void setCollection(const LCCollection *col)
  {
    if (_col==col) return;
    else _col=col;
    setCollection(col->getParameters().getStringVal(  LCIO::CellIDEncoding ));
  }
  
  inline void setValue(long64 cellID0, long64 cellID1)
  {
    _bf->setValue( ( cellID0 & 0xffffffff ) |  ( cellID1 << 32 ) );
  }

  BitField64& BF() {return *_bf;}

  private:
  BitField64 *_bf;
  const LCCollection *_col;
  
  };


  //base class for stuff common to all types of hits (Raw,Sim and CalorimeterHit)
  template <class HIT>
    class LCIO_hitVectorManipulation
    {
    public:
      //////////////////////////////////////////////////////////////////////////////////////////
      // 
      //  typedef for various kind of vectors of calorimeter hit and iterators
      //
      //  LCTypedVector is template <class T> class LCTypedVector : public  std::vector<T*> 
      //  with following constructor LCTypedVector( EVENT::LCCollection* col )
      //  and with accessor : EVENT::LCCollection* col();
      //
      //////////////////////////////////////////////////////////////////////////////////////////      
      typedef LCTypedVector<HIT> TCaloHitVector;      //type = vector<HIT*>
      typedef typename TCaloHitVector::iterator TCaloHitIterator;          //type = HIT**
      typedef typename TCaloHitVector::const_iterator const_TCaloHitIterator;          //type = const HIT**
      typedef std::pair< TCaloHitIterator, TCaloHitIterator > TCaloHitPairIterator;

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
	inline TCaloHitPairIterator equal_range(TCaloHitIterator  first, TCaloHitIterator  last, Pred &p)
	{
	  return std::equal_range(first, last, *first ,p);
	}

      //the main partition function
      template <class Pred>
	std::vector< TCaloHitPairIterator > calohitvec_partition(TCaloHitIterator  first, TCaloHitIterator  last, Pred &p)
	{
	  std::vector< TCaloHitPairIterator > res;
	  while (first != last )
	    {
	      TCaloHitPairIterator a=equal_range<Pred>(first,last,p);
	      res.push_back( a );
	      first=a.second;
	    }
	  return res;
	}

      template <class Pred>
	std::vector< TCaloHitPairIterator > calohitvec_sort_and_partition(TCaloHitVector &v,Pred &predicate)
	{
	  std::sort(v.begin(),v.end(), predicate );
	  return calohitvec_partition(v.begin(), v.end(), predicate );
	}
      
      //////////////////////////////////////////////////////////////////////////////////////////
      // 
      //  functions and functors to be used in algorithms (sorting, partitioning)
      //
      //////////////////////////////////////////////////////////////////////////////////////////
      //compare two pointers to calorimeter hits based on value in the cell id
      class CalorimeterHit_lessCellID 
      {
      public:
      CalorimeterHit_lessCellID(const std::string& encoder_str, const std::string& coding) 
	: m_codingString(coding) { m_decoder.setCollection(encoder_str);}
      CalorimeterHit_lessCellID( const LCCollection* col, const std::string& coding) 
	: m_codingString(coding) { m_decoder.setCollection(col);}

	
	bool operator()(const HIT* left , const HIT* right) 
	{ return getValue(left)<getValue(right);}
      private:
	//CellIDDecoder<HIT> m_decoder;
	static MyCellIDDecoder m_decoder;
	std::string m_codingString;
	
	long64 getValue(const HIT* h)
	{
	  m_decoder.setValue(h->getCellID0(),h->getCellID1());
	  return m_decoder.BF()[m_codingString];
	}
      };

      //////////////////////////////////////////////////////////////////////////////////////////
      // 
      //  sorting functions
      //
      //////////////////////////////////////////////////////////////////////////////////////////
      inline void CalorimeterHitVector_sortbyLayer(TCaloHitVector& v)
      {
	std::sort(v.begin(),v.end(), CalorimeterHit_lessCellID(v.col(),"K-1") );
      }

      //////////////////////////////////////////////////////////////////////////////////////////
      // 
      //  partitioning functions (changes vector ordering)
      //
      //////////////////////////////////////////////////////////////////////////////////////////
      //partition against layer number
      inline std::vector< TCaloHitPairIterator > partition_byLayer(TCaloHitVector& v)
      {
	CalorimeterHit_lessCellID predicate(v.col(),"K-1");
	return calohitvec_sort_and_partition(v,predicate);
      }

      
    };

  template <class HIT>
    MyCellIDDecoder LCIO_hitVectorManipulation<HIT>::CalorimeterHit_lessCellID::m_decoder=MyCellIDDecoder();

  //Stuff common to Sim and CalorimeterHit only : getPosition and getEnergy defined function
  //////////////////////////////////////////////////////////////////////////////////////////
  // 
  //  functions and functors to be used in algorithms (sorting, partitioning)
  //
  //////////////////////////////////////////////////////////////////////////////////////////
  //helper functions
  inline float dotProduct(const float *a, const float* b) { return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];}

  // compare two pointers to calorimeter hits based on the Z position of the pointed hits
  template <class HIT>
    inline bool CalorimeterHit_lessZ(const HIT* left , const HIT* right)
    {
      if ( 0 == left->getPosition() || 0 == right->getPosition() ) return true;
      return left->getPosition()[2]<right->getPosition()[2];
    } 

    
  //compare two pointers to calorimeter hits based on the position in a given direction
  template <class HIT>
    class CalorimeterHit_lessDir
    {
    public:
      CalorimeterHit_lessDir(float x, float y, float z) 
	{m_direction[0]=x; m_direction[1]=y; m_direction[2]=z;}
      CalorimeterHit_lessDir(float *dir)
	{m_direction[0]=dir[0]; m_direction[1]=dir[1]; m_direction[2]=dir[2];}
      bool operator()(const HIT* left , const HIT* right) {return getVal(left)<getVal(right);}
    private:
      float m_direction[3];
      float getVal(const HIT* hit) { if ( 0 == hit->getPosition() ) return 0; else return dotProduct(hit->getPosition(),m_direction);}
    };
  
  
  //compare two pointers to calorimeter hits by radius
  template <class HIT>
    inline bool CalorimeterHit_lessRadius(const HIT* left , const HIT* right)
    {
      if ( 0 == left->getPosition() || 0 == right->getPosition() ) return true;
      return dotProduct(left->getPosition(),left->getPosition()) < dotProduct(right->getPosition(),right->getPosition());
    }


  /*
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
  
  */

}

#endif

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

#include "SDHCAL_Util_CaloHitVector_template.h"

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
  // Explicit functions sorting
  //
  //////////////////////////////////////////////////////////////////////////////////////////
  //sort by Layer
  inline void CalorimeterHitVector_sortbyLayer(CaloHitVector& v) 
  {
    LCIO_hitVectorManipulation<CalorimeterHit> a;
    return a.CalorimeterHitVector_sortbyLayer(v);
  }
   inline void CalorimeterHitVector_sortbyLayer(SimCaloHitVector& v) 
  {
    LCIO_hitVectorManipulation<SimCalorimeterHit> a;
    return a.CalorimeterHitVector_sortbyLayer(v);
  }
   inline void CalorimeterHitVector_sortbyLayer(RawCaloHitVector& v) 
  {
    LCIO_hitVectorManipulation<RawCalorimeterHit> a;
    return a.CalorimeterHitVector_sortbyLayer(v);
  }
  
   //sort by Z
   inline void CalorimeterHitVector_sortbyZ(CaloHitVector& v) 
   {
     std::sort(v.begin(),v.end(),CalorimeterHit_lessZ<CalorimeterHit>);
   }
   inline void CalorimeterHitVector_sortbyZ(SimCaloHitVector& v) 
   {
     std::sort(v.begin(),v.end(),CalorimeterHit_lessZ<SimCalorimeterHit>);
   }
   

   //sort by position along given direction
   inline void CalorimeterHitVector_sortbyPosInDir(CaloHitVector& v,float *dir)
   {
     std::sort(v.begin(),v.end(),CalorimeterHit_lessDir<CalorimeterHit>(dir));
   }
   inline void CalorimeterHitVector_sortbyPosInDir(SimCaloHitVector& v,float *dir)
   {
     std::sort(v.begin(),v.end(),CalorimeterHit_lessDir<SimCalorimeterHit>(dir));
   }

   //sort by radius
   inline void CalorimeterHitVector_sortbyRadius(CaloHitVector& v)
   {
     std::sort(v.begin(),v.end(),CalorimeterHit_lessRadius<CalorimeterHit>);
   }
   inline void CalorimeterHitVector_sortbyRadius(SimCaloHitVector& v)
   {
     std::sort(v.begin(),v.end(),CalorimeterHit_lessRadius<SimCalorimeterHit>);
   }


  //////////////////////////////////////////////////////////////////////////////////////////
  //
  // Explicit functions sorting+partitioning
  //
  //////////////////////////////////////////////////////////////////////////////////////////
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
   

   //partition against layer number
   inline std::vector< CaloHitPairIterator > partition_byLayer(CaloHitVector& v)
   {
     LCIO_hitVectorManipulation<CalorimeterHit> a;
     return a.partition_byLayer(v);
   }
   inline std::vector< SimCaloHitPairIterator > partition_byLayer(SimCaloHitVector& v)
   {
     LCIO_hitVectorManipulation<SimCalorimeterHit> a;
     return a.partition_byLayer(v);
   }
   inline std::vector< RawCaloHitPairIterator > partition_byLayer(RawCaloHitVector& v)
   {
     LCIO_hitVectorManipulation<RawCalorimeterHit> a;
     return a.partition_byLayer(v);
   }
   
   
   //partition against Z position
   inline std::vector< CaloHitPairIterator > partition_byZ(CaloHitVector &v)
   { 
     LCIO_hitVectorManipulation<CalorimeterHit> a;
     return a.calohitvec_sort_and_partition(v,CalorimeterHit_lessZ<CalorimeterHit>);
   }
   inline std::vector< SimCaloHitPairIterator > partition_byZ(SimCaloHitVector &v)
   { 
     LCIO_hitVectorManipulation<SimCalorimeterHit> a;
     return a.calohitvec_sort_and_partition(v,CalorimeterHit_lessZ<SimCalorimeterHit>);
   }
   
   //partition against given direction 
   inline std::vector< CaloHitPairIterator > partition_byPosInDir(CaloHitVector& v,float *dir)
   {
     CalorimeterHit_lessDir<CalorimeterHit> predicate(dir);
     LCIO_hitVectorManipulation<CalorimeterHit> a;
     return a.calohitvec_sort_and_partition(v,predicate);
   }
   inline std::vector< SimCaloHitPairIterator > partition_byPosInDir(SimCaloHitVector& v,float *dir)
   {
     CalorimeterHit_lessDir<SimCalorimeterHit> predicate(dir);
     LCIO_hitVectorManipulation<SimCalorimeterHit> a;
     return a.calohitvec_sort_and_partition(v,predicate);
   }
   
}

#endif

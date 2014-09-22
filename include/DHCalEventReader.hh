#ifndef _DHCALEVENTREADER_H
#define _DHCALEVENTREADER_H
#include "lcio.h"
#include <stdio.h>
#include <bitset>
#include <vector>
#include "IO/LCReader.h"
#include "IMPL/LCTOOLS.h"
#include "IMPL/LCRunHeaderImpl.h" 
#include "IMPL/LCEventImpl.h" 

#include "EVENT/RawCalorimeterHit.h" 
#include "IMPL/LCGenericObjectImpl.h"
#include <IMPL/LCCollectionVec.h>
#include "IMPL/RawCalorimeterHitImpl.h"
#include "IMPL/LCEventImpl.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/LCFlagImpl.h"
#include <UTIL/LCSplitWriter.h>
#include "UTIL/CellIDDecoder.h"
#include <IO/LCRunListener.h>
#include <IO/LCEventListener.h>

#include <cstdlib>

using namespace std ;
using namespace lcio ;

class DHCalEventReader : public LCRunListener, public LCEventListener

{
public:
  //! Constructor

  DHCalEventReader();
  //! Destructor 

  ~DHCalEventReader();
  
  //! Get the current event. It does not read it 
  /** 
      @return an IMPL::LCEventImpl ptr
  */
  IMPL::LCEventImpl* getEvent() { return evt_;}
  
  //! Get the current Run Header. It does not read it 
  /** 
      @return an IMPL::LCRunHeaderImpl ptr
   */
  IMPL::LCRunHeaderImpl* getRunHeader(){return runh_;}

  void createEvent(int run,std::string det);

  void setEvent(IMPL::LCEventImpl* evt) {evt_=evt;}
 
  void open(std::string name);

  //! close the current file
  void close();

  void clear();

  int readStream();
  int readEvent();

  //! Read next run
  void readRun();

  void addRawOnlineRU(int* buf,int rusize);
  
  void buildEvent(bool rebuild=false,int nTrack=1);

  void openOutput(std::string filename);

  //! Write the RunHeader
  void writeRunHeader();

  void fillRunHeader();

  void writeEvent(int runNum,int evtNum,std::string srhcol);

  void closeOutput();

  void processEvent( LCEvent * evt )  ;
  //! Interface for readStream
  void processRunHeader( LCRunHeader* run) ;
  //! Interface for readStream
  void modifyEvent( LCEvent * evt ) { /* not needed */ ;}
  //! Interface for readStream
  void modifyRunHeader(LCRunHeader* run){ /* not needed */ ;}
  //void createRawCalorimeterHits();
  void createSimCalorimeterHits();
 protected:
 
 private:
  LCReader* lcReader_; /// LCIO Reader
  LCSplitWriter* lcWriter_; /// LCIO Writer
  IMPL::LCEventImpl* evt_; /// LCIO Event ptr
  IMPL::LCEventImpl* evtOutput_; /// LCIO Event output ptr
  IMPL::LCRunHeaderImpl* runh_; /// LCIO Run Heder ptr
  IMPL::LCCollectionVec *SimVec;

};

#endif

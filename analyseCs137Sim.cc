
// std includes
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <cmath>

// lcio includes
#include "lcio.h"
#include "IO/LCReader.h"
#include "IMPL/LCTOOLS.h"
#include "IMPL/LCCollectionVec.h"
//#include "EVENT/LCRunHeader.h" 
#include "EVENT/SimCalorimeterHit.h" 
#include "EVENT/LCGenericObject.h"

#include "SDHCAL_Simu_EventAnalyser.h"

using namespace std ;
using namespace lcio ; 

//Generic object step interface
class G4stepStoredInfo
{
public:
  G4stepStoredInfo(LCGenericObject& obj) : _ref(obj) {}
  int getCellID0() {return _ref.getIntVal(0);}
  int getNMCContributions() {return _ref.getIntVal(1);}
  float getStepEntryX() {return _ref.getFloatVal(0);}
  float getStepEntryY() {return _ref.getFloatVal(1);}
  float getStepEntryZ() {return _ref.getFloatVal(2);}
  float getStepExitX() {return _ref.getFloatVal(3);}
  float getStepExitY() {return _ref.getFloatVal(4);}
  float getStepExitZ() {return _ref.getFloatVal(5);}
  float getStepLength() {return _ref.getFloatVal(6);}
  bool isZeroLength() {return getStepEntryX()==getStepExitX() && getStepEntryY()==getStepExitY() && getStepEntryZ()==getStepExitZ();}
private:
  LCGenericObject& _ref;
};



struct compteurs
{
  int totalEvents,eventsWithHits;
  int totalSteps,startingSteps,forwardSteps,backwardSteps,zerolengthSteps;
  int nonStartingStepsSameDirection,nonStartingStepsOppositeDirection;
  compteurs() : totalEvents(0),eventsWithHits(0),totalSteps(0),startingSteps(0),forwardSteps(0), backwardSteps(0),zerolengthSteps(0),
		nonStartingStepsSameDirection(0),nonStartingStepsOppositeDirection(0) {}
  void printStat();

  map<unsigned int,int> distributionNombreHit;
  map<unsigned int,int> distributionNombreStep; 
  map<unsigned int, map<unsigned int,int> > distributionNstepsForNhits;
  map<float,int> stepEntryZ;
  void printDistributions();
};

void compteurs::printStat()
{
  cout << "Total number of events = " << totalEvents << endl; if (0==totalEvents) return;
  cout << "Events with hits = " << eventsWithHits << "  rate is " << double(eventsWithHits)/totalEvents << endl; 
  cout << "Total number of steps = " << totalSteps << endl;
  cout << "number of starting steps = " << startingSteps << endl;
  cout << "number of zero length steps = " << zerolengthSteps << endl;
  cout << "forward steps " << forwardSteps << "   backward steps "<< backwardSteps << endl;
  cout << "Among non starting steps " << nonStartingStepsSameDirection << " goes in the same Z direction than ending step and " 
       << nonStartingStepsOppositeDirection << " goes in the opposite Z direction than ending step." << endl;
}

void compteurs::printDistributions()
{
  cout << "Nombres hits ";
  for(map<unsigned int,int>::const_iterator it = distributionNombreHit.begin(); it != distributionNombreHit.end(); ++it)  
    cout << it->first << ":" << it->second << ",  ";
  cout << endl;
  cout << "Nombres steps ";
  for(map<unsigned int,int>::const_iterator it = distributionNombreStep.begin(); it != distributionNombreStep.end(); ++it)  
    cout << it->first << ":" << it->second << ",  ";
  cout << endl;
  cout << "distribution Nombres step pour " << endl;
  for(map<unsigned int, map<unsigned int,int> >::const_iterator itNhits=distributionNstepsForNhits.begin(); itNhits != distributionNstepsForNhits.end(); ++itNhits)
    {
      cout << itNhits->first << " hits : ";
      for(map<unsigned int,int>::const_iterator it = itNhits->second.begin(); it != itNhits->second.end(); ++it)  
	cout << it->first << ":" << it->second << ",  ";
      cout << endl;
    }
  int minEntryForPrint=startingSteps/20;
  if (minEntryForPrint<2) minEntryForPrint=2;
  cout << "step entry Z with minimum bin print set at 5% of total number = " << minEntryForPrint << " : " << endl;;
  int other=0;
  for (map<float,int>::const_iterator it = stepEntryZ.begin(); it != stepEntryZ.end(); ++it)
    {
      if (it->second >= minEntryForPrint)
	cout << it->first << ":" << it->second << ",  ";
      else
	other+=it->second;
    }
  cout << "other:"<<other<<endl;

}


struct myEventStuff
{
  static compteurs count;
  myEventStuff(LCEvent *event);
  vector<SimCalorimeterHit*> simHits;
  vector<LCGenericObject*> G4steps;
  void grainAmoudre();
  void lookAtSteps();
};

compteurs myEventStuff::count=compteurs();

template <class LCIOTRUC>
void fillVec(LCCollection *col,vector<LCIOTRUC*> &vec)
{
  if (NULL==col) return;
  int numElements = col->getNumberOfElements();
  for (int j(0); j < numElements; ++j) 
    {
      LCIOTRUC * obj = dynamic_cast<LCIOTRUC*>( col->getElementAt( j ) ) ;
      if (obj != NULL) vec.push_back(obj);
    }
}

myEventStuff::myEventStuff(LCEvent *event)
{
  ++count.totalEvents;
  try
    {
      LCCollection* simHitcol=event->getCollection( "SDHCAL_Proto_EndCap" );
      fillVec(simHitcol,simHits);
      ++count.distributionNombreHit[simHits.size()];
      fillVec(event->getCollection( "particleGenericObject" ), G4steps );
      if (! simHits.empty()) grainAmoudre();
    }
  catch( lcio::DataNotAvailableException& e )
    {
      cerr << e.what() << endl ;
    }
}

void myEventStuff::grainAmoudre()
{
  ++count.eventsWithHits;
  ++count.distributionNombreStep[G4steps.size()];
  ++count.distributionNstepsForNhits[simHits.size()][G4steps.size()];
  //cout << count.totalEvents << ":" << count.eventsWithHits << " events with " << simHits.size() << " hits and " << G4steps.size() << " steps." << endl;
  lookAtSteps();
}

void myEventStuff::lookAtSteps()
{  
  for (vector<LCGenericObject*>::iterator its=G4steps.begin(); its != G4steps.end(); ++its)
    {
      ++count.totalSteps;
      G4stepStoredInfo cestep(**its);
      if (cestep.isZeroLength())
	{
	  ++count.zerolengthSteps;
	  continue;
	}	    
      bool starting=true;
      for (vector<LCGenericObject*>::iterator it=G4steps.begin(); it != G4steps.end(); ++it)
	{
	  if (it==its) continue; 
	  G4stepStoredInfo step(**it);
	  if (step.isZeroLength()) continue; 
	  if (cestep.getStepEntryZ()==step.getStepExitZ()&&cestep.getStepEntryY()==step.getStepExitY()&&cestep.getStepEntryX()==step.getStepExitX())
	    {
	      starting=false;
	      if ((cestep.getStepEntryZ()-cestep.getStepExitZ())*(step.getStepEntryZ()-step.getStepExitZ())>0) ++count.nonStartingStepsSameDirection;
	      if ((cestep.getStepEntryZ()-cestep.getStepExitZ())*(step.getStepEntryZ()-step.getStepExitZ())<0) ++count.nonStartingStepsOppositeDirection;
	      if ((cestep.getStepEntryZ()-cestep.getStepExitZ())*(step.getStepEntryZ()-step.getStepExitZ())==0) 
		cout << "strange case (" 
		     << cestep.getStepEntryX()-cestep.getStepExitX()<<","<<cestep.getStepEntryY()-cestep.getStepExitY()<<","<<cestep.getStepEntryZ()-cestep.getStepExitZ()<<") et ("
		     << step.getStepEntryX()-step.getStepExitX()<<","<<step.getStepEntryY()-step.getStepExitY()<<","<<step.getStepEntryZ()-step.getStepExitZ()<<")" << endl;	  
	    }
	}
      if (starting)
	{
	  ++count.startingSteps;
	  ++count.stepEntryZ[cestep.getStepEntryZ()];
	  if (cestep.getStepEntryZ()<cestep.getStepExitZ()) ++count.forwardSteps;
	  if (cestep.getStepEntryZ()>cestep.getStepExitZ()) ++count.backwardSteps;
	}
      //else
      //{
      //  cout << "rejected step starts at Z=" << cestep.getStepEntryZ() << " and ends at Z=" << cestep.getStepExitZ() << endl; 
      //}
    }
}

int main(int argc, char** argv ){
  
  vector<string> inputFilesVec ;
  for (int i=1; i<argc; ++i)
    inputFilesVec.push_back(argv[i]);
  
  
  // ---------------------------------------------------------------------
  // end of command line argument parsing
  // ---------------------------------------------------------------------
 
  LCReader* lcReader = LCFactory::getInstance()->createLCReader() ;

  lcReader->open( inputFilesVec ) ;

  //lcReader->skipNEvents( 100 );
  
  LCEvent* event = NULL ;
  
  //The analyser
  SDHCAL_Simu_EventAnalyser analyser("SDHCAL_Proto_EndCap","HCALEndcap","particleGenericObject");
  //----------- the event loop -----------
  try{
    while( (event = lcReader->readNextEvent()) != 0 )
      {
	myEventStuff evt(event);
	analyser.newEvent(event);
      }
  }
  catch( lcio::IOException& e )
    //catch( ... )
    {
      cerr << "io error when reading data : " << e.what() << endl ;
    }
  // -------- end of event loop -----------

  myEventStuff::count.printStat();
  myEventStuff::count.printDistributions();

  analyser.printStat();

    // free lcReader
    lcReader->close();
    delete lcReader ;


    //return EXIT_FAILURE ;
    return EXIT_SUCCESS;
}


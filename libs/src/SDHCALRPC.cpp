#include "SDHCALRPC.hpp"

#include <vector>

#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4SDManager.hh"
#include "G4LogicalVolumeStore.hh"


void SDHCALRPC::buildStandardRPC()
{
  m_Layers.push_back( {"FrontCassetteAbsorber" , 2.500 , "SDHCAL_Steel316L" } ) ;
  m_Layers.push_back( {"Mask"                  , 1.600 , "SDHCAL_epoxy"     } ) ;	
  m_Layers.push_back( {"PCB"                   , 1.200 , "SDHCAL_g10"       } ) ;	
  m_Layers.push_back( {"MylarAnode"            , 0.050 , "G4_MYLAR"         } ) ;	
  m_Layers.push_back( {"GraphiteAnode"         , 0.050 , "G4_GRAPHITE"      } ) ;	
  m_Layers.push_back( {"ThinGlass"             , 0.700 , "G4_Pyrex_Glass"   } ) ;	
  m_Layers.push_back( {"GasGap"                , 1.200 , "SDHCAL_RPCGaz"    } ) ; //the layer called GasGap has a special Role
  m_Layers.push_back( {"ThickGlass"            , 1.100 , "G4_Pyrex_Glass"   } ) ;
  m_Layers.push_back( {"GraphiteCathode"       , 0.050 , "G4_GRAPHITE"      } ) ;	
  m_Layers.push_back( {"MylarCathode"          , 0.180 , "G4_MYLAR"         } ) ;
  m_Layers.push_back( {"BackCassetteAbsorber"  , 2.500 , "SDHCAL_Steel316L" } ) ;
  build();
}

void SDHCALRPC::buildOldStandardRPC()
{
  m_Layers.push_back( {"FrontCassetteAbsorber" , 2.500 , "SDHCAL_Steel316L_Old" } ) ;
  m_Layers.push_back( {"Mask"                  , 1.600 , "G4_Galactic"          } ) ;	
  m_Layers.push_back( {"PCB"                   , 1.200 , "G4_Galactic"          } ) ;	
  m_Layers.push_back( {"MylarAnode"            , 0.050 , "G4_MYLAR"             } ) ;	
  m_Layers.push_back( {"GraphiteAnode"         , 0.050 , "G4_GRAPHITE"          } ) ;	
  m_Layers.push_back( {"ThinGlass"             , 0.700 , "G4_Pyrex_Glass"       } ) ;	
  m_Layers.push_back( {"GasGap"                , 1.200 , "SDHCAL_RPCGaz"        } ) ; //the layer called GasGap has a special Role
  m_Layers.push_back( {"ThickGlass"            , 1.100 , "G4_Pyrex_Glass"       } ) ;
  m_Layers.push_back( {"GraphiteCathode"       , 0.050 , "G4_GRAPHITE"          } ) ;	
  m_Layers.push_back( {"MylarCathode"          , 0.180 , "G4_MYLAR"             } ) ;
  m_Layers.push_back( {"BackCassetteAbsorber"  , 2.500 , "SDHCAL_Steel316L_Old" } ) ;
  build();
}

void SDHCALRPC::buildWithScintillatorRPC()
{
  m_Layers.push_back( {"FrontCassetteAbsorber" , 2.500 , "SDHCAL_Steel316L" } ) ;
  m_Layers.push_back( {"MylarAnode"            , 0.015 , "G4_MYLAR"         } ) ;	
  m_Layers.push_back( {"GraphiteAnode"         , 0.050 , "G4_GRAPHITE"      } ) ;	
  m_Layers.push_back( {"ThinGlass"             , 0.700 , "G4_Pyrex_Glass"   } ) ;	
  m_Layers.push_back( {"GasGap"                , 1.200 , "SDHCAL_RPCGaz"    } ) ; //the layer called GasGap has a special Role
  m_Layers.push_back( {"ThickGlass"            , 1.100 , "G4_Pyrex_Glass"   } ) ;
  m_Layers.push_back( {"GraphiteCathode"       , 0.050 , "G4_GRAPHITE"      } ) ;	
  m_Layers.push_back( {"MylarCathode"          , 0.015 , "G4_MYLAR"         } ) ;
  m_Layers.push_back( {"Scintillator"          , 3.000 , "G4_POLYSTYRENE"   } ) ;
  m_Layers.push_back( {"BackCassetteAbsorber"  , 2.500 , "SDHCAL_Steel316L" } ) ;
  build();
}

SDHCALRPC::SDHCALRPC(G4int ID):m_ID(ID)
{
  m_Name="Cassette"+std::to_string(m_ID);
  G4LogicalVolumeStore* store = G4LogicalVolumeStore::GetInstance();
  if(store->GetVolume(m_Name, false))
  {
    G4cerr<< "WARNING : RPC with ID = " << m_ID << " already existing" <<G4endl;
    std::exit(-1);
  }
}

void SDHCALRPC::build()
{
  using CLHEP::mm;
  G4NistManager* man = G4NistManager::Instance();

  auto defaultMaterial = man->FindOrBuildMaterial("G4_Galactic");

  //compute total RPC cassette width
  G4double cassetteThickness = 0.0;
  for( const auto& layer : m_Layers) cassetteThickness += layer.m_Width*mm;
  m_SizeZ = cassetteThickness;

  //create logic RPC volume (indefined placement)
  G4Box* solidCassette = new G4Box(m_Name , m_SizeX/2, m_SizeY/2, cassetteThickness/2);
  m_LogicRPC = new G4LogicalVolume(solidCassette ,  defaultMaterial ,m_Name);

  //construct all the layers 
  G4double zPos = -cassetteThickness/2; //we start at front of the cassette
  for ( const auto& layer : m_Layers )
  {
    auto material = man->FindOrBuildMaterial(layer.m_Material);
    if ( ! material )
    {
      G4cerr << "Error : material " << layer.m_Material << " not known" << G4endl;
      std::exit(-1);
    }

    //create logic layer volume (indefined placement)
    auto solid = new G4Box(layer.m_Name , m_SizeX/2 , m_SizeY/2 , layer.m_Width*mm/2);
    auto logic = new G4LogicalVolume(solid , material ,layer.m_Name+std::to_string(m_ID));

    zPos += layer.m_Width*mm/2; //we are now at center of the current layer (where it has to be placed)

    //place the layer at zPos
    G4VPhysicalVolume* volume = new G4PVPlacement(nullptr , G4ThreeVector(0,0,zPos) , logic , layer.m_Name+std::to_string(m_ID) , m_LogicRPC,false,0,true);	//m_LogicRPC is the mother volume

    zPos += layer.m_Width*mm/2; //we are now at the back of the current layer

    if(layer.m_Name == "GasGap") //we keep trace of the gas gap
    {
      if(m_GasGap) //we only want one gas gap
      {
        G4cerr << "Error : there is more than one gas gap in the RPC" << G4endl;
        std::exit(-1);
      }

      m_GasGap = logic;
    }
  }

  if(!m_GasGap) //if we don't have the gas gap we have a problem
  {
    G4cerr << "Error : no gas gap in the RPC" << G4endl;
    std::exit(-1);
  }
}

G4VPhysicalVolume* SDHCALRPC::createPhysicalVolume(G4RotationMatrix* rot , G4ThreeVector trans , G4LogicalVolume* motherLogic)
{
  return new G4PVPlacement(rot , trans , m_LogicRPC , m_Name , motherLogic , false , 0 , true);
}

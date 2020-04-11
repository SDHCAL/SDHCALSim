set(INCLUDES
    include/SDHCALActionInitialization.hpp
    include/SDHCALPrimaryGeneratorAction.hpp
    include/SDHCALRPC.hpp
    include/SDHCALSteppingAction.hpp
    include/SDHCALRootWriter.hpp
    include/SDHCALHit.hpp
    include/SDHCALLcioWriter.hpp
    include/SDHCALPrimaryGeneratorActionMessenger.hpp
    include/SDHCALGun.hpp
    include/SDHCALRPCSensitiveDetector.hpp
    include/MyRandom.hpp
    include/SDHCALTrackingAction.hpp
    include/SDHCALStackingAction.hpp
    include/SDHCALRunAction.hpp
    include/SDHCALEventAction.hpp
    include/SDHCALDetectorConstruction.hpp 
    include/SDHCALRun.hpp
    include/SDHCALMaterials.hpp
    )
    
set(SRC
    src/SDHCALActionInitialization.cpp
    src/SDHCALPrimaryGeneratorAction.cpp 
    src/SDHCALRPC.cpp
    src/SDHCALSteppingAction.cpp
    src/SDHCALRootWriter.cpp
    src/SDHCALHit.cpp
    src/SDHCALLcioWriter.cpp
    src/SDHCALPrimaryGeneratorActionMessenger.cpp
    src/SDHCALGun.cpp
    src/SDHCALRPCSensitiveDetector.cpp
    src/SDHCALTrackingAction.cpp
    src/SDHCALStackingAction.cpp
    src/SDHCALRunAction.cpp
    src/SDHCALEventAction.cpp 
    src/SDHCALDetectorConstruction.cpp 
    src/SDHCALRun.cpp
    src/SDHCALMaterials.cpp
    src/MyRandom.cpp
    )    

add_library(MyLib STATIC ${SRC})
target_include_directories(MyLib PRIVATE PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> PUBLIC LCIO_LIBRARY_DIRS PUBLIC ${INCLUDE_OUTPUT_DIR} PUBLIC ${Geant4_INCLUDE_DIRS} PUBLIC ${ROOT_INCLUDE_DIRS})
target_link_libraries(MyLib PUBLIC ${ROOT_LIBRARIES} PUBLIC ${Geant4_LIBRARIES} PUBLIC ${LCIO_LIBRARIES})
set_target_properties(MyLib PROPERTIES PUBLIC_HEADER "${INCLUDES}")

install(TARGETS MyLib RUNTIME DESTINATION bin LIBRARY DESTINATION ${LIBRARY_OUTPUT_DIR} ARCHIVE DESTINATION ${ARCHIVE_OUTPUT_DIR} PUBLIC_HEADER DESTINATION include)

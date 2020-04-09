set(INCLUDES
    include/SDHCALPrimaryGeneratorAction.h 
    include/SDHCALRPC.h
    include/SDHCALSteppingAction.h
    include/SDHCALRootWriter.h
    include/SDHCALHit.h
    include/SDHCALLcioWriter.h
    include/SDHCALPrimaryGeneratorActionMessenger.h
    include/SDHCALGun.h
    include/SDHCALRPCSensitiveDetector.h
    include/MyRandom.h
    include/SDHCALTrackingAction.h
    include/SDHCALStackingAction.h
    include/SDHCALRunAction.h
    include/SDHCALEventAction.h 
    include/SDHCALDetectorConstruction.h 
    include/SDHCALRun.h
    include/SDHCALMaterials.h
    )
    
set(SRC
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

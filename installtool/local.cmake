#you should have setup LCIOdir, ROOTdir and GEANT4dir environment variable and source init.sh before using this file

SET( CMAKE_PREFIX_PATH 
	$ENV{LCIO};
	$ENV{ROOTSYS};
	$ENV{GEANT4dir};
	/usr;
CACHE PATH "CMAKE_PREFIX_PATH" FORCE )


option(USE_CXX11 "Use cxx11" True)


1) Installation

1.1) Installation with ilcsoft


     	mkdir build ; cd build ;
	cmake -C $ILCSOFT/ILCSoft.cmake ..;
	make -j4 install ;
	cd ..

1.2) Installation without ilcsoft. you'll need LCIO, ROOT and GEANT4 software

     	export LCIOdir=/path/to/lcio/installation/directory
	export ROOTdir=/path/to/root/installation/directory
	export GEANT4dir=/path/to/geant4/installation/directory
	source  installtool/init.sh
	mkdir build ; cd build ;
	cmake -C ../installtool/local.cmake .. ;
	make -j4 install ;
	cd ..
	

2) running
set the correct `SIMEXE` env variable in script/example.py

to run :

	./script/example.py

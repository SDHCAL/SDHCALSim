	mkdir build ; cd build ;
	cmake -C $ILCSOFT/ILCSoft.cmake ;
	make -j4 install ;
	cd ..

set the correct `SIMEXE` env variable in script/example.py

to run :

	./script/example.py

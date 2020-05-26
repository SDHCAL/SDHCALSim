
![Linux](https://github.com/RPClab/SDHCALSim/workflows/Linux/badge.svg?branch=master)   ![macOS](https://github.com/RPClab/SDHCALSim/workflows/macOS/badge.svg?branch=master)   ![Windows](https://github.com/RPClab/SDHCALSim/workflows/Windows/badge.svg?branch=master)

# Installation

## Installation with ilcsoft

mkdir build ; cd build ;
cmake -C $ILCSOFT/ILCSoft.cmake ..;
make -j4 install ;
cd ..

## Installation without ilcsoft. you'll need LCIO, ROOT and GEANT4 software

export LCIOdir=/path/to/lcio/installation/directory
source  installtool/init.sh
mkdir build ; cd build ;
cmake -C ../installtool/local.cmake .. ;
make -j4 install ;
cd ..

# Running

./script/example.py

#In case, you're not using the ilcsoft setup,
#source this script to setup for run and compilation
#
# LCIOdir   environment variable should point to the directory where lcio is installed
# ROOTdir   environment variable should point to the directory where root is installed
# GEANT4dir environment variable should point to the directory where Geant4 is installed

export LCIO="$LCIOdir"
export PYTHONPATH="$LCIO/python:$LCIO/python/examples:$PYTHONPATH"
export PATH="$LCIO/tools:$LCIO/bin:$PATH"
export LD_LIBRARY_PATH="$LCIO/lib:$LD_LIBRARY_PATH"

cd $ROOTdir
source bin/thisroot.sh
cd -

cd $GEANT4dir
source bin/geant4.sh
cd -

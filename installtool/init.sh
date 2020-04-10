#In case, you're not using the ilcsoft setup,
#source this script to setup for run and compilation
#
# LCIOdir   environment variable should point to the directory where lcio is installed

export LCIO="$LCIOdir"
export PYTHONPATH="$LCIO/python:$LCIO/python/examples:$PYTHONPATH"
export PATH="$LCIO/tools:$LCIO/bin:$PATH"
export LD_LIBRARY_PATH="$LCIO/lib:$LD_LIBRARY_PATH"

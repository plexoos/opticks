#!/bin/bash -l 
usage(){ cat << EOU
G4CXOpticks_setGeometry_Test.sh
===================================

Test of geometry conversions in isolation. 

WIP: get this to work with the FewPMT geometry coming from PMTSim 

EOU
}

REALPATH=$(find $PWD -name $(basename $BASH_SOURCE))
REALDIR=$(dirname $REALPATH)  

#source $(dirname $BASH_SOURCE)/../../bin/GEOM_.sh   # change the geometry with geom_ 
source ~/.opticks/GEOM/GEOM.sh   # sets GEOM envvar 

case $GEOM in 
   FewPMT) geomscript=$REALDIR/../../u4/tests/FewPMT.sh ;;
esac

if [ -n "$geomscript" -a -f "$geomscript" ]; then 
    echo $BASH_SOURCE : GEOM $GEOM : sourcing geomscript $geomscript
    source $geomscript
else
    echo $BASH_SOURCE : GEOM $GEOM : no geomscript    
fi 



export GProperty_SIGINT=1
#export NTreeBalance__UnableToBalance_SIGINT=1
#export BFile__preparePath_SIGINT=1
export GGeo__save_SIGINT=1

export G4CXOpticks__setGeometry_saveGeometry=~/.opticks/GEOM/$GEOM


loglevels(){
   export Dummy=INFO
   export G4CXOpticks=INFO
   #export X4PhysicalVolume=INFO
   #export SOpticksResource=INFO
   export CSGFoundry=INFO
   export GSurfaceLib=INFO
}
loglevels

env | grep =INFO


bin=G4CXOpticks_setGeometry_Test

export FOLD=/tmp/$USER/opticks/$bin
mkdir -p $FOLD

vars="REALPATH REALDIR GEOM FOLD bin geomscript"

defarg=info_dbg
arg=${1:-$defarg}


if [ "${arg/info}" != "$arg" ]; then 
    for var in $vars ; do printf "%30s : %s \n" "$var" "${!var}" ; done 
fi 

if [ "${arg/run}" != "$arg" ]; then 
    $bin
    [ $? -ne 0 ] && echo $BASH_SOURCE : run error && exit 1 
fi 

if [ "${arg/dbg}" != "$arg" ]; then 
    export TAIL="-o run"
    case $(uname) in 
       Darwin) lldb__ $bin  ;; 
       Linux)  gdb__  $bin ;;
    esac
    [ $? -ne 0 ] && echo $BASH_SOURCE : dbg error && exit 2
fi 

if [ "${arg/ana}" != "$arg" ]; then 
    ${IPYTHON:-ipython} --pdb -i $bin.py 
    [ $? -ne 0 ] && echo $BASH_SOURCE : ana error && exit 3
fi 

exit 0


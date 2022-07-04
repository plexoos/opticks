#!/bin/bash -l 
usage(){ cat << EOU
gxs.sh : Opticks CX GPU simulation starting from Geant4 geometry auto-translated to CSG
==========================================================================================

::

    cd ~/opticks/g4cx   # gx
    ./gxs.sh 
    ./gxs.sh dbg

EOU
}

source ../bin/GEOM_.sh 
source ../bin/OPTICKS_INPUT_PHOTON.sh 

loglevels()
{
    export Dummy=INFO
    #export U4VolumeMaker=INFO

    export SEvt=INFO
    #export Ctx=INFO
    #export QSim=INFO
    #export QEvent=INFO 
    #export CSGOptiX=INFO
    #export G4CXOpticks=INFO 
    #export X4PhysicalVolume=INFO
}
loglevels



defarg="run"
arg=${1:-$defarg}

if [ "${arg/run}" != "$arg" ]; then 
    G4CXSimulateTest
    [ $? -ne 0 ] && echo $BASH_SOURCE run error && exit 1 
fi 

if [ "${arg/dbg}" != "$arg" ]; then 
    case $(uname) in
        Linux) gdb G4CXSimulateTest -ex r  ;;
        Darwin) lldb__ G4CXSimulateTest ;; 
    esac
    [ $? -ne 0 ] && echo $BASH_SOURCE dbg error && exit 2 
fi


export FOLD=/tmp/$USER/opticks/G4CXSimulateTest/$GEOM   # corresponds to SPath::Resolve("$DefaultOutputDir", DIRPATH) 

if [ "${arg/ana}" != "$arg" ]; then 
    ${IPYTHON:-ipython} --pdb -i tests/G4CXSimulateTest.py     
    [ $? -ne 0 ] && echo $BASH_SOURCE ana error && exit 3 
fi 

if [ "${arg/grab}" != "$arg" ]; then 
    source ../bin/rsync.sh $FOLD 
fi 


exit 0 


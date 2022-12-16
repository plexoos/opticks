#!/bin/bash -l 
usage(){ cat << EOU
U4SimtraceTest.sh
==========================

::

    N=0 ./U4SimtraceTest.sh 
    N=1 ./U4SimtraceTest.sh 

    APID=173 PLAB=1 BGC=yellow ./U4SimtraceTest.sh ana

Z-changing big bouncers::

    N=0 APID=256 PLAB=1 BGC=white ./U4SimtraceTest.sh ana
    N=1 APID=261 PLAB=1 BGC=white ./U4SimtraceTest.sh ana

Grab the custom boundary status for each point::

    In [25]: t.aux[261,:32,1,3].copy().view(np.int8)[::4].copy().view("|S32")
    Out[25]: array([b'TTTZNZRZNZA'], dtype='|S32')


::


    N=0 APID=726 BPID=-1 AOPT=nrm FOCUS=0,10,185 ./U4SimtraceTest.sh ana

    N=1 APID=-1 BPID=726 BOPT=nrm FOCUS=0,10,185 ./U4SimtraceTest.sh ana



two_pmt::

    FOCUS=0,0,255 ./U4SimtraceTest.sh ana


    N=0 APID=813 AOPT=idx BGC=yellow FOCUS=0,0,255 ./U4SimtraceTest.sh ana
    N=0 APID=813 AOPT=ast BGC=yellow FOCUS=0,0,255 ./U4SimtraceTest.sh ana

    N=1 BPID=748 BOPT=ast,nrm BGC=yellow FOCUS=0,0,255 ./U4SimtraceTest.sh ana
    N=1 BPID=150 BOPT=nrm,ast BGC=yellow FOCUS=0,0,255 ./U4SimtraceTest.sh ana
  
EOU
}

bin=U4SimtraceTest

layout=two_pmt  
#layout=one_pmt

apid=-1
bpid=-1

export LAYOUT=$layout
export VERSION=${N:-0}

export GEOM=hamaLogicalPMT

export GEOMFOLD=/tmp/$USER/opticks/GEOM/$GEOM
export BASE=$GEOMFOLD/$bin
export FOLD=$BASE/$VERSION   ## controls where the executable writes geometry
export SFOLD=$BASE/0
export TFOLD=$BASE/1

export AFOLD=$GEOMFOLD/U4SimulateTest/ALL0
export BFOLD=$GEOMFOLD/U4SimulateTest/ALL1   # SEL1 another possibility 
export APID=${APID:-$apid}   ## NB APID for photons from ALL0
export BPID=${BPID:-$bpid}   ## NB BPID for photons from ALL1
export LOC=skip   # python ana level presentation 

geomscript=$GEOM.sh 

if [ -f "$geomscript" ]; then  
    source $geomscript $VERSION $LAYOUT
else
    echo $BASH_SOURCE : no gemoscript $geomscript
fi 

loglevels()
{
    export U4VolumeMaker=INFO
}
loglevels


log=${bin}.log
logN=${bin}_$VERSION.log

defarg="run_ana"
arg=${1:-$defarg}

if [ "${arg/run}" != "$arg" ]; then
    [ -f "$log" ] && rm $log 
    $bin
    [ $? -ne 0 ] && echo $BASH_SOURCE run error && exit 1 
    [ -f "$log" ] && echo $BASH_SOURCE rename log $log to logN $logN && mv $log $logN    
fi 

if [ "${arg/dbg}" != "$arg" ]; then
    [ -f "$log" ] && rm $log 
    case $(uname) in 
        Darwin) lldb__ $bin ;;
        Linux)   gdb__ $bin ;;
    esac
    [ $? -ne 0 ] && echo $BASH_SOURCE dbg error && exit 2
fi 

if [ "${arg/ana}" != "$arg"  ]; then
    [ "$arg" == "nana" ] && export MODE=0
    ${IPYTHON:-ipython} --pdb -i $bin.py 
    [ $? -ne 0 ] && echo $BASH_SOURCE ana error && exit 3
fi 

if [ "$arg" == "pvcap" -o "$arg" == "pvpub" -o "$arg" == "mpcap" -o "$arg" == "mppub" ]; then
    export CAP_BASE=$SFOLD/figs
    export CAP_REL=U4SimtraceTest
    export CAP_STEM=$GEOM
    case $arg in  
       pvcap) source pvcap.sh cap  ;;  
       mpcap) source mpcap.sh cap  ;;  
       pvpub) source pvcap.sh env  ;;  
       mppub) source mpcap.sh env  ;;  
    esac
    if [ "$arg" == "pvpub" -o "$arg" == "mppub" ]; then 
        source epub.sh 
    fi  
fi 

exit 0 

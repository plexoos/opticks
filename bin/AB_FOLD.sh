#!/bin/bash 
usage(){ cat << EOU
AB_FOLD.sh
============

This script is expected to be sourced, it exports envvars A_FOLD and B_FOLD
which are used for comparing output arrays for example by:

1. u4/tests/U4RecorderTest_ab.sh 
2. sysrap/dv.sh 

The A_FOLD and B_FOLD variables exported are sensitive to the FOLD_MODE
control envvar.   

FOLD_MODE:TMP
    used for everyday smaller samples that can easily be recreated, 
    for which being lost on rebooting the machine is not a problem

FOLD_MODE:KEEP
    used for larger samples which prefer not to loose at every reboot

EOU
}

export FOLD_MODE=${FOLD_MODE:-TMP}


tmp=/tmp/$USER/opticks
keep=/usr/local/opticks/tests


A_FOLD_TMP=$tmp/GeoChain/BoxedSphere/CXRaindropTest
B_FOLD_TMP=$tmp/U4RecorderTest/ShimG4OpAbsorption_ORIGINAL_ShimG4OpRayleigh_ORIGINAL
#B_FOLD_TMP=/tmp/$USER/opticks/U4RecorderTest/ShimG4OpAbsorption_FLOAT_ShimG4OpRayleigh_FLOAT

A_FOLD_KEEP=$keep/GeoChain/BoxedSphere/CXRaindropTest
B_FOLD_KEEP=$keep/U4RecorderTest

A_FOLD_LOGF=$tmp/U4RecorderTest/ShimG4OpAbsorption_FLOAT_ShimG4OpRayleigh_FLOAT
B_FOLD_LOGF=$tmp/U4RecorderTest/ShimG4OpAbsorption_ORIGINAL_ShimG4OpRayleigh_ORIGINAL

A_FOLD_GEOM=$tmp/U4RecorderTest/ShimG4OpAbsorption_FLOAT_ShimG4OpRayleigh_FLOAT/$GEOM
B_FOLD_GEOM=$tmp/U4RecorderTest/ShimG4OpAbsorption_FLOAT_ShimG4OpRayleigh_FLOAT/$GEOM

A_FOLD_GXS=$tmp/G4CXSimulateTest/$GEOM
B_FOLD_GXS=$tmp/U4RecorderTest/ShimG4OpAbsorption_FLOAT_ShimG4OpRayleigh_FLOAT/$GEOM



case $FOLD_MODE in
  TMP)  export A_FOLD=$A_FOLD_TMP  ; export B_FOLD=$B_FOLD_TMP  ;; 
  KEEP) export A_FOLD=$A_FOLD_KEEP ; export B_FOLD=$B_FOLD_KEEP ;; 
  LOGF) export A_FOLD=$A_FOLD_LOGF ; export B_FOLD=$B_FOLD_LOGF ;; 
  GEOM) export A_FOLD=$A_FOLD_GEOM ; export B_FOLD=$B_FOLD_GEOM ;; 
   GXS) export A_FOLD=$A_FOLD_GXS ; export B_FOLD=$B_FOLD_GXS ;; 
esac

vars="BASH_SOURCE FOLD_MODE A_FOLD B_FOLD"
for var in $vars ; do printf "%25s : %s \n" "$var" "${!var}" ; done 

echo A_FOLD $A_FOLD
du -hs $A_FOLD
du -hs $A_FOLD/*.npy

echo B_FOLD $B_FOLD
du -hs $B_FOLD
du -hs $B_FOLD/*.npy


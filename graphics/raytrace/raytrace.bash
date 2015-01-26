# === func-gen- : graphics/raytrace/raytrace fgp graphics/raytrace/raytrace.bash fgn raytrace fgh graphics/raytrace
raytrace-src(){      echo graphics/raytrace/raytrace.bash ; }
raytrace-source(){   echo ${BASH_SOURCE:-$(env-home)/$(raytrace-src)} ; }
raytrace-vi(){       vi $(raytrace-source) ; }
raytrace-usage(){ cat << EOU

Test Combination of Assimp and OptiX
======================================

::

    raytrace-;raytrace-- __dd__Geometry__AD__lvOIL0xbf5e0b8 --dim=256x256


* press "ctrl" and drag up/down to zoom out/on 

EOU
}
raytrace-bdir(){ echo $(local-base)/env/graphics/raytrace ; }
raytrace-sdir(){ echo $(env-home)/graphics/raytrace ; }
raytrace-cd(){  cd $(raytrace-sdir); }
raytrace-scd(){  cd $(raytrace-sdir); }
raytrace-bcd(){  cd $(raytrace-bdir); }

raytrace-env(){      
    elocal-  
    assimp-
    optix-
    optix-export
    export-
    export-export
}

raytrace-name(){ echo RayTrace ; }

raytrace-cmake(){
   local iwd=$PWD

   local bdir=$(raytrace-bdir)
   mkdir -p $bdir

   raytrace-bcd
  
   cmake -DCMAKE_BUILD_TYPE=Debug -DOptiX_INSTALL_DIR=$(optix-install-dir) -DCUDA_NVCC_FLAGS="-ccbin /usr/bin/clang --use_fast_math" $(raytrace-sdir) 

   cd $iwd
}


raytrace-make(){
   local iwd=$PWD
   raytrace-bcd
   make $*

   cd $iwd
}



raytrace-bin(){ echo $(raytrace-bdir)/$(raytrace-name) ; }
raytrace-run-(){ cat << EOC
  DYLD_LIBRARY_PATH=$(assimp-prefix)/lib $(raytrace-bin) $*
EOC
}

raytrace-run(){
   $FUNCNAME- $*
   $FUNCNAME- $* | sh 
}




raytrace--(){
  raytrace-make
  raytrace-run $* 
}

raytrace-dbg(){
  raytrace-bcd
  DYLD_LIBRARY_PATH=$(assimp-prefix)/lib lldb $(raytrace-bin)
}





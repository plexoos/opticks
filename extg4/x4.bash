x4-source(){ echo $BASH_SOURCE ; }
x4-vi(){ vi $(x4-source) ; }
x4-env(){  olocal- ; opticks- ; }
x4-usage(){ cat << EOU

X4 Usage 
===================


EOU
}


x4-dir(){ echo $(dirname $(x4-source)) ; }
x4-cd(){  cd $(x4-dir) ; }
x4--(){   opticks-- $(x4-dir) ; }

x4-names()
{
   local iwd=$PWD
   g4- 
   cd $(g4-dir)/source/persistency/gdml/src

   echo "BooleanSolid" 

   grep ==\"G4 G4GDMLWriteSolids.cc | perl -n -e 'm,\"G4(\w*)\", && print "$1\n" ' -  
   cd $iwd
}


x4-foreach()
{
   local func=$1
   printf "// generated by $func $(date) \n" ; 
   local name
   x4-names | while read name ; do 
      $func $name
   done
}

x4-entity(){ x4-foreach $FUNCNAME- ; }
x4-entity-(){
   local name=$1
   local n=$(printf "n.push_back(\"G4%s\")" $name )
   local t=$(printf "t.push_back(_G4%s)" $name )
   printf "    %-35s ; %-35s ; \n" $n $t 
}

x4-enum(){  x4-foreach $FUNCNAME- ; }
x4-enum-(){ printf "    _G4%s ,\n" $1 ; }

x4-case(){  x4-foreach $FUNCNAME- ; }
x4-case-(){ 
   local name=$1
   local a=$(printf "case _G4$name" )
   local b=$(printf "convert$name()" )
   local c="; break ;"
   printf "    %-25s : %-30s %s \n" "$a" "$b" "$c"  
} 


x4-convert-cc(){  x4-foreach $FUNCNAME- ; }
x4-convert-cc-(){ cat << EOC
void X4Solid::convert$1()
{  
    const G4${1}* const solid = static_cast<const G4${1}*>(m_solid);
    assert(solid); 
    assert(0) ; 
}
EOC
}
x4-convert-hh(){  x4-foreach $FUNCNAME- ; }
x4-convert-hh-(){ cat << EOC
        void convert$1();
EOC
}

x4-include(){  x4-foreach $FUNCNAME- ; }
x4-include-(){ echo "#include \"G4$1.hh\"" ; }







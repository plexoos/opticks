#pragma once

#include <vector>
#include <string>

#include "X4_API_EXPORT.hh"

class Opticks; 
class G4VSolid ; 

struct nnode ; 
class NPYMeta ; 
class NCSG ; 
class NCSGData ; 
class NCSGList ; 

/**
X4CSG
=======

* Used for testing single G4VSolid in isolation
* Applied to all solids using codegen to construct mains 
  for every solid, see x4gen-

**/

struct X4_API X4CSG 
{
    static const std::string HEAD ; 
    static const std::string TAIL ; 

    static void Serialize( const G4VSolid* solid, Opticks* ok, const char* csgpath );
    static void GenerateTest( const G4VSolid* solid, Opticks* ok, const char* prefix, unsigned lvidx );  
    static const char* GenerateTestPath( const char* prefix, unsigned lvidx, const char* ext ) ; 

    static G4VSolid* MakeContainer(const G4VSolid* solid, float scale) ; 
    std::string desc() const ;
    std::string configuration(const char* csgpath) const ;

    X4CSG(const G4VSolid* solid, Opticks* ok );

    void init();
    void checkTree() const ;
    void setIndex(unsigned index_);

    void configure( NPYMeta* meta );
    void dump(const char* msg="X4CSG::dump");
    std::string save(const char* csgpath) ;
    void loadcheck(const char* csgpath) const ;

    void generateTestMain( std::ostream& out ) const ;
    void dumpTestMain(const char* msg="X4CSG::dumpTestMain") const ;
    void writeTestMain( const char* path ) const ;

    int              verbosity ; 
    const G4VSolid*  solid ; 
    Opticks*         ok ; 
    std::string      gdml ; 
    const G4VSolid*  container ; 
    const char*      solid_boundary ; 
    const char*      container_boundary ; 
    nnode*           nraw ;   // unbalanced
    nnode*           nsolid ; 
    nnode*           ncontainer ; 
    NCSG*            csolid ; 
    NCSG*            ccontainer ; 
    NCSGList*        ls ; 
    std::vector<NCSG*> trees ;
    int              index ; 

};






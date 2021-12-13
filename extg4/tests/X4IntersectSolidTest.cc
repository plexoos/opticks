/**
X4IntersectTest
=================

Used from script extg4/xxs.sh 

**/
#include "OPTICKS_LOG.hh"
#include "SSys.hh"
#include "SStr.hh"
#include "SPath.hh"

#include "X4Intersect.hh"
#include "X4SolidMaker.hh"
#include "X4Mesh.hh"

#ifdef WITH_PMTSIM
#include "PMTSim.hh"
#endif

const G4VSolid* GetSolid(const char* name)
{
    const G4VSolid* solid = nullptr ; 
    if(X4SolidMaker::CanMake(name))
    {
        solid = X4SolidMaker::Make(name); 
    }
    else
    {
#ifdef WITH_PMTSIM
        solid = PMTSim::GetSolid(name); 
#endif
    }
    return solid ; 
}

int main(int argc, char** argv)
{
    OPTICKS_LOG(argc, argv); 

    //const char* geom_default = "pmt_solid" ; // not appropriate for default to require j/PMTSim 
    const char* geom_default = "Orb" ; 
    const char* geom = SSys::getenvvar("GEOM", geom_default );  

    std::stringstream ss ; 
    ss << "creator:X4IntersectTest" << std::endl ; 
    ss << "geom:" << geom << std::endl ; 
#ifdef WITH_PMTSIM
    ss << "info:WITH_PMTSIM " << std::endl ; 
#else
    ss << "info:noPMTSIM " << std::endl ; 
#endif
    std::string meta = ss.str(); 
    LOG(info) << meta ; 


    std::vector<std::string> names ; 
    SStr::Split(geom,',',names);  

    LOG(info) << " geom " << geom << " names.size " << names.size() ; 


    const char* base = "$TMP/extg4/X4IntersectTest" ; 

    for(unsigned i=0 ; i < names.size() ; i++)
    {
        const std::string& name_ = names[i]; 
        const char* name = name_.c_str() ; 
        const G4VSolid* solid = GetSolid(name); 
        if( solid == nullptr ) LOG(fatal) << "failed to GetSolid for name " << name ; 
        assert( solid );   


        X4Intersect::Scan(solid, name, base, meta ); 

        int create_dirs = 1 ; // 1:filepath 
        const char* meshpath = SPath::Resolve(base, name, "X4Mesh", "mesh.gltf", create_dirs ); 
        X4Mesh::Save(solid, meshpath ); 

    }
    return 0 ; 
}


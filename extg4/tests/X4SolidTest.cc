#include "G4Sphere.hh"
#include "X4Solid.hh"
#include "OPTICKS_LOG.hh"

int main(int argc, char** argv)
{
    OPTICKS_LOG_COLOR__(argc, argv);

    G4Sphere* sp = X4Solid::MakeSphere("demo_sphere"); 

    std::cout << *sp << std::endl ; 

    X4Solid* so = new X4Solid(sp) ; 

    LOG(info) << so->desc() ; 

 
    return 0 ; 
}

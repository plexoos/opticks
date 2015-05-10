
#include "NPY.hpp"
#include "G4StepNPY.hpp"
#include "Lookup.hpp"



int main()
{
    NPY* npy = NPY::load("cerenkov", "1");

    G4StepNPY cs(npy);

    const char* apath = "/tmp/ChromaMaterialMap.json" ;
    const char* aprefix = "/dd/Materials/" ;
    Lookup lookup ; 
    lookup.create(apath, aprefix, apath, aprefix);

    for(unsigned int a=0; a < 35 ; a++ )
    {
        int b = lookup.a2b(a);
        printf("  a => b %u -> %d \n", a, b );
    }



    return 0 ;
}

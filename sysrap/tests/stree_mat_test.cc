#include "stree.h"
#include "NPX.h"

int main(int argc, char** argv)
{
    const char* ssbase = "$HOME/.opticks/GEOM/$GEOM/CSGFoundry/SSim" ; 

    stree st ; 
    int rc = st.load(ssbase); 
    if( rc != 0 ) return rc ; 

    
    //std::cout << "st.desc" << std::endl << st.desc() << std::endl ; 

    std::cout << "st.material.desc" << std::endl << st.material->desc() << std::endl ; 
    std::cout << "st.desc_mt" << std::endl << st.desc_mt() << std::endl; 

    std::cout << "st.surface.desc" << std::endl << st.surface->desc() << std::endl ; 
    std::cout << "st.desc_bd" << std::endl << st.desc_bd() << std::endl; 


    // arrays directly under SSim all from old X4/GGeo workflow
    const NP* _oldmat = NP::Load(ssbase, "oldmat.npy"); 
    const NP* _oldsur = NP::Load(ssbase, "oldsur.npy"); 
    const NP* _oldbnd = NP::Load(ssbase, "bnd.npy"); 
    const NP* _oldoptical = NP::Load(ssbase, "optical.npy"); 


    // TODO: these should be auto-created 
    const NP* bnd = st.make_bnd() ;  
    const NP* bd  = st.make_bd() ; 
    const NP* optical = st.make_optical() ;  


    NPFold* fold = new NPFold ; 

    fold->add("oldmat", _oldmat ); 
    fold->add("oldsur", _oldsur ); 
    fold->add("oldbnd", _oldbnd ); 
    fold->add("oldoptical", _oldoptical ); 

    // TODO: tidy these into fold populated by stree::save 
    // then can eliminate this executable can just 
    // directly load from the persisted GEOM 
 
    fold->add("mat", st.mat ); 
    fold->add("sur", st.sur ); 
    fold->add("rayleigh",  st.rayleigh  ); 
    fold->add("energy",  st.energy ); 
    fold->add("wavelength",  st.wavelength ); 

    fold->add("bnd", bnd ); 
    fold->add("bd",  bd  ); 
    fold->add("optical",  optical  ); 

    fold->save("$FOLD"); 
 
    return 0 ; 
}

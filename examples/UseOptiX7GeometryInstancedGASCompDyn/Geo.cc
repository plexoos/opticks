
#include <iostream>
#include <cstring>

#include "Util.h"
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

#include "Geo.h"

Geo* Geo::fGeo = NULL ; 

Geo::Geo(const char* spec_, const char* geometry_) 
   :
   spec(strdup(spec_)),  
   geometry(strdup(geometry_))
{
   fGeo = this ; 
   init();
}

void Geo::init()
{
    float tminf(0.1) ; 
    float tmaxf(10000.f) ; 

    if(strcmp(geometry, "sphere_containing_grid_of_two_radii_spheres_compound") == 0)
    {
        init_sphere_containing_grid_of_two_radii_spheres_compound(tminf, tmaxf);
    }
    else if(strcmp(geometry, "sphere_containing_grid_of_two_radii_spheres") == 0)
    {
        init_sphere_containing_grid_of_two_radii_spheres(tminf, tmaxf);
    }
    else if(strcmp(geometry, "sphere") == 0 )
    {
        init_sphere(tminf, tmaxf);
    }
    else if(strcmp(geometry, "sphere_two") == 0 )
    {
        init_sphere_two(tminf, tmaxf);
    }
    else
    {
        init_sphere_containing_grid_of_two_radii_spheres(tminf, tmaxf);
    }

    float top_extent = getTopExtent(); 
    tmin = top_extent*tminf ; 
    tmax = top_extent*tmaxf ; 
    std::cout 
        << "Geo::init" 
        << " top_extent " << top_extent  
        << " tminf " << tminf 
        << " tmin " << tmin 
        << " tmaxf " << tmaxf 
        << " tmax " << tmax 
        << std::endl 
        ; 

    float e_tminf = Util::GetEValue<float>("TMIN", -1.0) ; 
    if(e_tminf > 0.f )
    {
        tmin = top_extent*e_tminf ; 
        std::cout << "Geo::init e_tminf TMIN " << e_tminf << " override tmin " << tmin << std::endl ; 
    }
    
    float e_tmaxf = Util::GetEValue<float>("TMAX", -1.0) ; 
    if(e_tmaxf > 0.f )
    {
        tmax = top_extent*e_tmaxf ; 
        std::cout << "Geo::init e_tmaxf TMAX " << e_tmaxf << " override tmax " << tmax << std::endl ; 
    }
}

void Geo::init_sphere_containing_grid_of_two_radii_spheres(float& tminf, float& tmaxf)
{
    std::cout << "Geo::init_sphere_containing_grid_of_two_radii_spheres " << spec << std::endl ; 

    float ias_extent = 10.f ; 
    float ias_step = 2.f ; 

    makeGAS(0.7f); 
    makeGAS(1.0f); 
    std::vector<unsigned> gas_modulo = {0, 1} ;

    makeGAS(ias_extent*2.0f); 
    std::vector<unsigned> gas_single = {2} ;

    makeIAS(ias_extent, ias_step, gas_modulo, gas_single ); 

    setTop(spec); 

    tminf = 0.75f ; 
    tmaxf = 10000.f ; 
}


void Geo::init_sphere_containing_grid_of_two_radii_spheres_compound(float& tminf, float& tmaxf)
{
    std::cout << "Geo::init_sphere_containing_grid_of_two_radii_spheres_compound " << spec << std::endl ; 

    float ias_extent = 10.f ; 
    float ias_step = 2.f ; 

    makeGAS(0.7f, 0.35f); 
    makeGAS(1.0f, 0.5f); 
    std::vector<unsigned> gas_modulo = {0, 1} ;

    makeGAS(ias_extent*2.0f); 
    std::vector<unsigned> gas_single = {2} ;

    makeIAS(ias_extent, ias_step, gas_modulo, gas_single ); 

    setTop(spec); 

    tminf = 0.75f ;   // <-- so can see inside the big sphere  
    tmaxf = 10000.f ; 
}

void Geo::init_sphere(float& tminf, float& tmaxf)
{
    std::cout << "Geo::init_sphere" << std::endl ; 
    makeGAS(100.f); 
    setTop("g0"); 

    tminf = 1.60f ;   //  hmm depends on viewpoint, aiming to cut into the sphere with the tmin
    tmaxf = 10000.f ; 
}
void Geo::init_sphere_two(float& tminf, float& tmaxf)
{
    std::cout << "Geo::init_sphere_two" << std::endl ; 
    std::vector<float> extents = {100.f, 90.f, 80.f, 70.f, 60.f, 50.f   } ; 
    makeGAS(extents); 
    setTop("g0"); 

    float top_extent = getTopExtent(); 
    std::cout << "Geo::init_sphere_two top_extent " << top_extent  << std::endl ; 

    tminf = 1.50f ;   //  hmm depends on viewpoint, aiming to cut into the sphere with the tmin
    tmaxf = 10000.f ; 
}

Geo* Geo::Get()
{
    return fGeo ; 
}

AS* Geo::getTop() const 
{
    return top ; 
}

float Geo::getTopExtent() const 
{
    assert(top); 
    return top ? top->extent0 : -1.f ;  
}

void Geo::setTop(const char* spec)
{
    AS* a = getAS(spec); 
    setTop(a); 
}

AS* Geo::getAS(const char* spec) const 
{
   assert( strlen(spec) > 1 ); 
   char c = spec[0]; 
   assert( c == 'i' || c == 'g' ); 
   int idx = atoi( spec + 1 ); 

   std::cout << "Geo::getAS " << spec << " c " << c << " idx " << idx << std::endl ; 

   AS* a = nullptr ; 
   if( c == 'i' )
   {
       const IAS& ias = getIAS(idx); 
       a = (AS*)&ias ; 
   } 
   else if( c == 'g' )
   {
       const GAS& gas = getGAS(idx); 
       a = (AS*)&gas ; 
   }

   if(a)
   {
       std::cout << "Geo::getAS " << spec << " a->extent0 " << a->extent0 << std::endl ; 
   } 
   return a ; 
}

void Geo::setTop(AS* top_)
{
    top = top_ ; 
}

/**
Geo::makeGAS
---------------

GAS can hold multiple bbox, but for now use just use one each 
with symmetric extent about the origin.

**/
void Geo::makeGAS(float extent)
{
    std::cout << "Geo::makeGAS extent " << extent << std::endl ; 
    std::vector<float> extents ;
    extents.push_back(extent);  
    makeGAS(extents); 
}

void Geo::makeGAS(float extent0, float extent1)
{
    std::cout << "Geo::makeGAS extent0 " << extent0 << " extent1 " << extent1  << std::endl ; 
    std::vector<float> extents ;
    extents.push_back(extent0);  
    extents.push_back(extent1);  
    makeGAS(extents); 
}
void Geo::makeGAS(const std::vector<float>& extents)
{
    std::cout << "Geo::makeGAS extents.size() " << extents.size() << " : " ; 
    for(unsigned i=0 ; i < extents.size() ; i++) std::cout << extents[i] << " " ; 
    std::cout << std::endl ; 

    std::vector<float> bb ; 

    float fudge = Util::GetEValue("FUDGE", 1.5f)  ; 
    std::cout << "Geo::makeGAS fudge " << fudge << " : " ; 

    float extent0 = extents[0]*fudge ; 

    for(unsigned i=0 ; i < extents.size() ; i++)
    {
        float extent = extents[i]*fudge ;
        assert( extent <= extent0 );  

        std::cout << extent << " " ; 
 
        bb.push_back(-extent); 
        bb.push_back(-extent); 
        bb.push_back(-extent); 
        bb.push_back(+extent); 
        bb.push_back(+extent); 
        bb.push_back(+extent); 
    }
    std::cout << std::endl ; 

    GAS gas = GAS::Build(bb); 
    gas.extent0 = extent0 ; 
    gas.extents = extents ; 
    vgas.push_back(gas); 
}

unsigned Geo::getNumGAS() const 
{
    return vgas.size() ; 
}
unsigned Geo::getNumIAS() const 
{
    return vias.size() ; 
}

unsigned Geo::getNumHitgroupRec() const 
{
    unsigned num_rec = 0 ; 
    for(unsigned i=0 ; i < getNumGAS() ; i++)
    {
        const GAS& gas = getGAS(i) ;    
        num_rec += gas.num_sbt_rec ;  
    }
    return num_rec ; 
}


const GAS& Geo::getGAS(int gas_idx_) const
{
    unsigned gas_idx = gas_idx_ < 0 ? vgas.size() + gas_idx_ : gas_idx_ ;  
    assert( gas_idx < vgas.size() ); 
    return vgas[gas_idx] ; 
}
const IAS& Geo::getIAS(int ias_idx_) const
{
    unsigned ias_idx = ias_idx_ < 0 ? vias.size() + ias_idx_ : ias_idx_ ;  
    assert( ias_idx < vias.size() ); 
    return vias[ias_idx] ; 
}

float unsigned_as_float( unsigned u ) 
{
    union { unsigned u; int i; float f; } uif ;   
    uif.u = u  ;   
    return uif.f ; 
}

/**
Geo::makeIAS
-------------

Create vector of transfoms and creat IAS from that.
Currently a 3D grid of translate transforms with all available GAS repeated modulo

**/

void Geo::makeIAS(float extent, float step, const std::vector<unsigned>& gas_modulo, const std::vector<unsigned>& gas_single )
{
    int n=int(extent) ;   // 
    int s=int(step) ; 

    unsigned num_gas = getNumGAS(); 
    unsigned num_gas_modulo = gas_modulo.size() ; 
    unsigned num_gas_single = gas_single.size() ; 

    std::cout << "Geo::makeIAS"
              << " num_gas " << num_gas
              << " num_gas_modulo " << num_gas_modulo
              << " num_gas_single " << num_gas_single
              << std::endl
              ;

    for(unsigned i=0 ; i < num_gas_modulo ; i++ ) assert(gas_modulo[i] < num_gas) ; 
    for(unsigned i=0 ; i < num_gas_single ; i++ ) assert(gas_single[i] < num_gas) ; 

    std::vector<glm::mat4> trs ; 

    for(int i=0 ; i < int(num_gas_single) ; i++)
    {
        unsigned idx = trs.size(); 
        unsigned instance_id = idx ; 
        unsigned gas_idx = gas_single[i] ; 

        glm::mat4 tr(1.f) ;
        tr[0][3] = unsigned_as_float(instance_id); 
        tr[1][3] = unsigned_as_float(gas_idx) ;
        tr[2][3] = unsigned_as_float(0) ;   
        tr[3][3] = unsigned_as_float(0) ;   

        trs.push_back(tr); 
    }

    for(int i=-n ; i <= n ; i+=s ){
    for(int j=-n ; j <= n ; j+=s ){
    for(int k=-n ; k <= n ; k+=s ){

        glm::vec3 tlat(i*1.f,j*1.f,k*1.f) ; 
        glm::mat4 tr(1.f) ;
        tr = glm::translate(tr, tlat );

        unsigned idx = trs.size(); 
        unsigned instance_id = idx ; 
        unsigned gas_modulo_idx = idx % num_gas_modulo ; 
        unsigned gas_idx = gas_modulo[gas_modulo_idx] ; 

        tr[0][3] = unsigned_as_float(instance_id); 
        tr[1][3] = unsigned_as_float(gas_idx) ;
        tr[2][3] = unsigned_as_float(0) ;   
        tr[3][3] = unsigned_as_float(0) ;   

        trs.push_back(tr); 
    }
    }
    }

    IAS ias = IAS::Build(trs); 
    ias.extent0 = extent ; 
    vias.push_back(ias); 
}


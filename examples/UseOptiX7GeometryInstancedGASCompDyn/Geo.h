#pragma once
#include <array>
#include <vector>
#include <optix.h>
#include <glm/glm.hpp>

#include "IAS.h"
#include "GAS.h"

struct Geo
{
    static Geo* fGeo ; 
    static Geo* Get();  

    Geo(const char* spec_, const char* geometry_);

    void init();
    void init_sphere_containing_grid_of_spheres(float& tminf, float& tmaxf, unsigned layers);
    void init_sphere(float& tminf, float& tmaxf, unsigned layers);

    unsigned getNumGAS() const ; 
    unsigned getNumIAS() const ; 

    unsigned getNumBI() const ;
    unsigned getNumBI(unsigned gas_idx) const ;
    unsigned getOffsetBI(unsigned gas_idx) const ;
    void     dumpOffsetBI() const ;

    const GAS& getGAS(int gas_idx_) const ; 
    const IAS& getIAS(int ias_idx_) const ; 


    void makeGAS(float outer_extent, unsigned layers);
    void makeGAS(const std::vector<float>& extents);
    void addGAS(const GAS& gas);

    void makeIAS_Grid( std::array<int,9>& grid, const std::vector<unsigned>& gas_modulo, const std::vector<unsigned>& gas_single );


    void writeIAS(unsigned ias_idx, const char* dir) const ;
    void writeGAS(unsigned gas_idx, const char* dir) const ; 
    void write(const char* prefix) const ; 

    static void WriteNP( const char* dir, const char* name, float* data, int ni, int nj, int nk ); 


    AS* getAS(const char* spec) const ;
    void setTop(const char* spec) ; 
    void setTop(AS* top_) ; 
    AS* getTop() const ; 

    void setTopExtent(float top_extent_); 
    float getTopExtent() const ; 

    const char* spec = nullptr ; 
    const char* geometry = nullptr ; 
    AS* top = nullptr ; 

    float tmin = 0.f ; 
    float tmax = 1e16f ; 
    float top_extent = 100.f ; 

    std::vector<GAS> vgas ; 
    std::vector<IAS> vias ; 
    std::vector<unsigned> nbis ; 

};

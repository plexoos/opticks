#pragma once

#include <vector>
class GCache ; 
class GItemList ;
template <typename T> class GPropertyMap ;

#include "GPropertyLib.hh"

class GScintillatorLib : public GPropertyLib {
    public:
        static const char* slow_component; 
        static const char* fast_component; 
        static const char* keyspec ;
    public:
        void save();
        void dump(const char* msg="GScintillatorLib::dump");
        static GScintillatorLib* load(GCache* cache);
    public:
        GScintillatorLib(GCache* cache, unsigned int icdf_length=4096);
    public:
        void add(GPropertyMap<float>* scint);
        unsigned int getNumScintillators();
    public:
       // concretization of GPropertyLib
       void defineDefaults(GPropertyMap<float>* defaults); 
       void import();
       void sort();
       NPY<float>* createBuffer();
       GItemList*  createNames();
    private:
        void init();
    public:
        GProperty<float>* constructReemissionCDF(GPropertyMap<float>* pmap);
        GProperty<float>* constructInvertedReemissionCDF(GPropertyMap<float>* pmap);
    private:
        unsigned int m_icdf_length ; 

};

inline GScintillatorLib::GScintillatorLib( GCache* cache, unsigned int icdf_length) 
    :
    GPropertyLib(cache, "GScintillatorLib"),
    m_icdf_length(icdf_length)
{
    init();
}

inline unsigned int GScintillatorLib::getNumScintillators()
{
    return getNumRaw();
}



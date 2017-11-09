#pragma once

#include <vector>

/** 
GSurfaceLib
==============

Skin and Border surfaces have an associated optical surface 
that is lodged inside GPropertyMap
in addition to 1(for skin) or 2(for border) volume names

* huh : where are these names persisted ?
    

**/

class NMeta ; 
struct guint4 ; 
class GOpticalSurface ; 
class GSkinSurface ; 
class GBorderSurface ; 
class GItemList ; 

#include "GPropertyLib.hh"
#include "GGEO_API_EXPORT.hh"
#include "GGEO_HEAD.hh"

class GGEO_API GSurfaceLib : public GPropertyLib {
   public:
       static const char* propertyName(unsigned int k);
       // 4 standard surface property names : interleaved into float4 wavelength texture
  public:
       static const char* detect ;
       static const char* absorb ;
       static const char* reflect_specular ;
       static const char* reflect_diffuse ;
  public:
       static const char* extra_x ; 
       static const char* extra_y ; 
       static const char* extra_z ; 
       static const char* extra_w ; 
  public:
       static const char* AssignSurfaceType( NMeta* surfmeta );
       static const char* BORDERSURFACE ;  
       static const char* SKINSURFACE ;  
       static const char* TESTSURFACE ;  
  public:
       static const char* BPV1 ;  
       static const char* BPV2 ;  
       static const char* SSLV ;  
  public:
       // some model-mismatch translation required for surface properties
       static const char* EFFICIENCY ; 
       static const char* REFLECTIVITY ; 
   public:
       static const char*  SENSOR_SURFACE ;
       static float        SURFACE_UNSET ; 
       static const char* keyspec ;
   public:
       void save();
       static GSurfaceLib* load(Opticks* ok);
   public:
       GSurfaceLib(Opticks* ok, GSurfaceLib* basis=NULL); 
       GSurfaceLib(GSurfaceLib* other, GDomain<float>* domain=NULL, GSurfaceLib* basis=NULL );  // interpolating copy ctor
   private:
       void init();
       void initInterpolatingCopy(GSurfaceLib* src, GDomain<float>* domain);
   public:
       GSurfaceLib* getBasis();
       void         setBasis(GSurfaceLib* basis);
   public:
       void Summary(const char* msg="GSurfaceLib::Summary");
       void dump(const char* msg="GSurfaceLib::dump");
       void dump(GPropertyMap<float>* surf);
       void dump(GPropertyMap<float>* surf, const char* msg);
       void dump(unsigned int index);
   public:
       // concretization of GPropertyLib
       void defineDefaults(GPropertyMap<float>* defaults); 
       NPY<float>* createBuffer();
       NMeta*      createMeta();
       GItemList*  createNames();
   public:
       NPY<float>* createBufferForTex2d();
       NPY<float>* createBufferOld();
   public:
      // methods for debug
       void setFakeEfficiency(float fake_efficiency);
       GPropertyMap<float>* makePerfect(const char* name, float detect_, float absorb_, float reflect_specular_, float reflect_diffuse_);
       void addPerfectSurfaces();
    public:
        // methods to assist with de-conflation of surface props and location
        void addBorderSurface(GPropertyMap<float>* surf, const char* pv1, const char* pv2);
        void addSkinSurface(GPropertyMap<float>* surf, const char* sslv_ );
    public:
        void add(GSkinSurface* ss);
        void add(GBorderSurface* bs);
        void add(GPropertyMap<float>* surf);
   private:
       void addDirect(GPropertyMap<float>* surf);
   public:
       void sort();
       bool operator()(const GPropertyMap<float>* a_, const GPropertyMap<float>* b_);
   public:
       guint4               getOpticalSurface(unsigned int index);  // zero based index
       GPropertyMap<float>* getSurface(unsigned int index);         // zero based index
       GPropertyMap<float>* getSurface(const char* name);        
       GPropertyMap<float>* getSensorSurface(unsigned int offset=0);  // 0: first, 1:second 
       bool hasSurface(unsigned int index); 
       bool hasSurface(const char* name); 
   private:
       guint4               createOpticalSurface(GPropertyMap<float>* src);
       GPropertyMap<float>* createStandardSurface(GPropertyMap<float>* src);
       bool checkSurface( GPropertyMap<float>* surf);
   public:
      // unlike former GBoundaryLib optical buffer one this is surface only
       NPY<unsigned int>* createOpticalBuffer();  
       void importOpticalBuffer(NPY<unsigned int>* ibuf);
       void saveOpticalBuffer();
       void loadOpticalBuffer();
       void setOpticalBuffer(NPY<unsigned int>* ibuf);
       NPY<unsigned int>* getOpticalBuffer();
   public:
       unsigned int getNumSurfaces();
       bool isSensorSurface(unsigned int surface); // name suffix based, see AssimpGGeo::convertSensor
   public:
       void import();
   private:
       void importOld();
       void importForTex2d();
       void import( GPropertyMap<float>* surf, float* data, unsigned int nj, unsigned int nk, unsigned int jcat=0 );
   private:
       std::vector<GPropertyMap<float>*>       m_surfaces ; 
       float                                   m_fake_efficiency ; 
       NPY<unsigned int>*                      m_optical_buffer ; 
       GSurfaceLib*                            m_basis ; 


};

#include "GGEO_TAIL.hh"



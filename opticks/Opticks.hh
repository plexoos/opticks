#pragma once

#include <string>
#include <cstring>
#include <vector>
#include <glm/glm.hpp>

template <typename> class OpticksCfg ;

class TorchStepNPY ; 
class NumpyEvt ;
class NLog ;
class Parameters ; 
class OpticksResource ; 

#include "OpticksPhoton.h"

class Opticks {
       friend class OpticksCfg<Opticks> ; 
   public:
       static const char* COMPUTE ; 

   public:
       static const char* ZERO_ ;
       static const char* CERENKOV_ ;
       static const char* SCINTILLATION_ ;

       static const char* MISS_ ;
       static const char* BULK_ABSORB_ ;
       static const char* BULK_REEMIT_ ;
       static const char* BULK_SCATTER_ ;
       static const char* SURFACE_DETECT_ ;
       static const char* SURFACE_ABSORB_ ;
       static const char* SURFACE_DREFLECT_ ;
       static const char* SURFACE_SREFLECT_ ;
       static const char* BOUNDARY_REFLECT_ ;
       static const char* BOUNDARY_TRANSMIT_ ;
       static const char* TORCH_ ;
       static const char* NAN_ABORT_ ;
       static const char* BAD_FLAG_ ;
       static const char* OTHER_ ;

       static const char* cerenkov_ ;
       static const char* scintillation_ ;
       static const char* torch_ ;
       static const char* other_ ;

       static const char* BNDIDX_NAME_ ;
       static const char* SEQHIS_NAME_ ;
       static const char* SEQMAT_NAME_ ;

       static const char* COMPUTE_MODE_ ;
       static const char* INTEROP_MODE_ ;
       static const char* CFG4_MODE_ ;

       enum {
                COMPUTE_MODE = 0x1 << 1, 
                INTEROP_MODE = 0x1 << 2, 
                CFG4_MODE = 0x1 << 3
            }; 
         
       static const char* SourceType(int code);
       static const char* SourceTypeLowercase(int code);
       static unsigned int SourceCode(const char* type);
       static const char* Flag(const unsigned int flag);
       static std::string FlagSequence(const unsigned long long seqhis);
   public:
       // wavelength domain
       static unsigned int DOMAIN_LENGTH ; 
       static float        DOMAIN_LOW ; 
       static float        DOMAIN_HIGH ; 
       static float        DOMAIN_STEP ; 
       static glm::vec4    getDefaultDomainSpec();

   public:
       Opticks(const char* envprefix="OPTICKS_", const char* logname="opticks.log", const char* loglevel="info");
       void configure(int argc, char** argv);
   private:
       void init();
       void preconfigure(int argc, char** argv);
   public:
       void Summary(const char* msg="Opticks::Summary");
   public:
       const char* getIdPath();
       const char* getLastArg();
       int         getLastArgInt();
   public:
       void setGeocache(bool geocache=true);
       bool isGeocache();
       void setInstanced(bool instanced=true);
       bool isInstanced();
   public:
       std::string getRelativePath(const char* path); 
   public:
       void setMode(unsigned int mode);
       void setDetector(const char* detector); 

   public:
       OpticksCfg<Opticks>* getCfg();
       OpticksResource*     getResource(); 
       Parameters*          getParameters();
       std::string          getModeString();
   public:
       TorchStepNPY*        makeSimpleTorchStep();
       NumpyEvt*            makeEvt(); 
   public:
       const glm::vec4&  getTimeDomain();
       const glm::vec4&  getSpaceDomain();
       const glm::vec4&  getWavelengthDomain();
       const glm::ivec4& getSettings();
   public:
       void setSpaceDomain(const glm::vec4& pd);
       void dumpDomains(const char* msg="Opticks::dumpDomains");
   public:
       unsigned int getRngMax();
       unsigned int getBounceMax();
       unsigned int getRecordMax();
       float        getEpsilon();
   public:
       unsigned int getSourceCode();
       std::string getSourceType();
   public:
       bool isCompute();
       bool isInterop();
       bool isCfG4();
   public:
       // methods required by Cfg listener classes
       void configureF(const char* name, std::vector<float> values);
       void configureI(const char* name, std::vector<int> values);
       void configureS(const char* name, std::vector<std::string> values);
   private:
       void configureDomains();
       void setCfg(OpticksCfg<Opticks>* cfg);
   private:
       const char*      m_envprefix ;
       OpticksResource* m_resource ; 
 
       const char* m_logname  ; 
       const char* m_loglevel  ; 
       NLog*       m_log ; 
   private:
       bool        m_compute ; 
       bool        m_geocache ; 
       bool        m_instanced ; 
       const char* m_lastarg ; 
   private:
       OpticksCfg<Opticks>* m_cfg ; 
       Parameters*          m_parameters ; 
       const char*          m_detector ; 
       glm::vec4            m_time_domain ; 
       glm::vec4            m_space_domain ; 
       glm::vec4            m_wavelength_domain ; 

   private:
       glm::ivec4       m_settings ; 
       //NB avoid duplication between here and OpticksCfg , only things that need more control need be here

       unsigned int         m_mode ; 

};



inline Opticks::Opticks(const char* envprefix, const char* logname, const char* loglevel)
     :
       m_envprefix(strdup(envprefix)),
       m_resource(NULL),

       m_logname(strdup(logname)),
       m_loglevel(strdup(loglevel)),
       m_log(NULL),

       m_compute(false),
       m_geocache(false),
       m_instanced(true),

       m_lastarg(NULL),

       m_cfg(NULL),
       m_parameters(NULL),
       m_detector(NULL),
       m_mode(0u)
{
       init();
}

inline void Opticks::setCfg(OpticksCfg<Opticks>* cfg)
{
    m_cfg = cfg ; 
}
inline OpticksCfg<Opticks>* Opticks::getCfg()
{
    return m_cfg ; 
}

inline Parameters* Opticks::getParameters()
{
    return m_parameters ; 
}

inline OpticksResource* Opticks::getResource()
{
    return m_resource  ; 
}



inline const char* Opticks::getLastArg()
{
   return m_lastarg ; 
}





inline void Opticks::setMode(unsigned int mode)
{
    m_mode = mode ; 
}


inline bool Opticks::isCompute()
{
    return (m_mode & COMPUTE_MODE) != 0  ; 
}
inline bool Opticks::isInterop()
{
    return (m_mode & INTEROP_MODE) != 0  ; 
}
inline bool Opticks::isCfG4()
{
    return (m_mode & CFG4_MODE) != 0  ; 
}



inline void Opticks::setGeocache(bool geocache)
{
    m_geocache = geocache ; 
}
inline bool Opticks::isGeocache()
{
    return m_geocache ;
}

inline void Opticks::setInstanced(bool instanced)
{
   m_instanced = instanced ;
}
inline bool Opticks::isInstanced()
{
   return m_instanced ; 
}










inline const glm::vec4& Opticks::getTimeDomain()
{
    return m_time_domain ; 
}
inline const glm::vec4& Opticks::getSpaceDomain()
{
    return m_space_domain ; 
}
inline const glm::vec4& Opticks::getWavelengthDomain()
{
    return m_wavelength_domain ; 
}
inline const glm::ivec4& Opticks::getSettings()
{
    return m_settings ; 
}


inline void Opticks::setDetector(const char* detector)
{
    m_detector = detector ? strdup(detector) : NULL ; 
}
inline void Opticks::setSpaceDomain(const glm::vec4& sd)
{
    m_space_domain.x = sd.x  ; 
    m_space_domain.y = sd.y  ; 
    m_space_domain.z = sd.z  ; 
    m_space_domain.w = sd.w  ; 
}


inline void Opticks::configureS(const char* name, std::vector<std::string> values)
{
}

inline void Opticks::configureI(const char* name, std::vector<int> values)
{
}
 

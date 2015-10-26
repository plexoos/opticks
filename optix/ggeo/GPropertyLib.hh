#pragma once

#include <map>
#include <string>

#include "GDomain.hh"
#include "GPropertyMap.hh"

template <typename T> class NPY ;

class GCache ; 
class GItemList ; 

class GPropertyLib {
    public:
        static unsigned int DOMAIN_LENGTH ; 
        static float        DOMAIN_LOW ; 
        static float        DOMAIN_HIGH ; 
        static float        DOMAIN_STEP ; 
    public:
        unsigned int getIndex(const char* shortname);
        GPropertyLib(GCache* cache, const char* type);
        std::string getCacheDir();
        const char* getType();
        virtual ~GPropertyLib();

        void setStandardDomain(GDomain<float>* standard_domain);
        void setDefaults(GPropertyMap<float>* defaults);
    private:
        void init();
    public:
        GDomain<float>*      getStandardDomain();
        unsigned int         getStandardDomainLength();
        GPropertyMap<float>* getDefaults();
        GProperty<float>*    getDefaultProperty(const char* name);
    public:
        // defaults need to be set in concrete subclass
        virtual void defineDefaults(GPropertyMap<float>* defaults) = 0 ; 
        virtual void import() = 0 ; 
    public:
        GProperty<float>*    getPropertyOrDefault(GPropertyMap<float>* pmap, const char* pname);
        GProperty<float>*    getProperty(GPropertyMap<float>* pmap, const char* dkey);
        GProperty<float>*    makeConstantProperty(float value);
        GProperty<float>*    makeRampProperty();
    public:
        void setKeyMap(const char* spec);
        const char* getLocalKey(const char* dkey); // map standard -> local keys 
    public:
        // another classes need access to "shape" of the standardization
        static GDomain<float>* getDefaultDomain();
    public:
        bool hasBuffer();
        std::string  getBufferName();
        NPY<float>*  getBuffer();
        GItemList*   getNames();
    public:
       void saveToCache();
       void loadFromCache();
    public:
        void         setBuffer(NPY<float>* buf);
        void         setNames(GItemList* names);
    protected:
        GCache*                              m_cache ; 
        NPY<float>*                          m_buffer ; 
        GItemList*                           m_names ; 
        const char*                          m_type ; 
        GDomain<float>*                      m_standard_domain ;  
    private:
        GPropertyMap<float>*                 m_defaults ;  
        std::map<std::string, std::string>   m_keymap ; //  
};

inline GPropertyLib::GPropertyLib(GCache* cache, const char* type) 
     :
     m_cache(cache),
     m_buffer(NULL),
     m_names(NULL),
     m_type(strdup(type)),
     m_standard_domain(NULL),
     m_defaults(NULL)
{
     init();
}


inline const char* GPropertyLib::getType()
{
    return m_type ; 
}

inline GPropertyLib::~GPropertyLib()
{
}

inline void GPropertyLib::setStandardDomain(GDomain<float>* standard_domain)
{
    m_standard_domain = standard_domain ; 
}
inline GDomain<float>* GPropertyLib::getStandardDomain()
{
    return m_standard_domain ;
}

inline void GPropertyLib::setDefaults(GPropertyMap<float>* defaults)
{
    m_defaults = defaults ;
}
inline GPropertyMap<float>* GPropertyLib::getDefaults()
{
    return m_defaults ;
}

inline void GPropertyLib::setBuffer(NPY<float>* buf)
{
    m_buffer = buf ;
}
inline NPY<float>* GPropertyLib::getBuffer()
{
    return m_buffer ;
}
inline bool GPropertyLib::hasBuffer()
{
    return m_buffer != NULL ; 
}


inline void GPropertyLib::setNames(GItemList* names)
{
    m_names = names ;
}
inline GItemList* GPropertyLib::getNames()
{
    return m_names ;
}






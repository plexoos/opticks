#pragma once

struct qevent ; 
struct quad4 ;
struct qat4 ; 
struct quad6 ;
struct NP ; 

//template <typename T> struct Tran ; 
template <typename T> struct QBuf ; 

#include <vector>
#include <string>
#include "plog/Severity.h"
#include "QUDARAP_API_EXPORT.hh"

/**
QEvent
=======

TODO: follow OEvent technique of initial allocation and resizing at each event 

Unlike typical CPU side event classes am thinking of QEvent/qevent being rather "static" 
with long lived buffers of defined maximum capacity that get reused for each launch.

**/

struct QUDARAP_API QEvent
{
    static const plog::Severity LEVEL ; 
    static const QEvent* INSTANCE ; 
    static const QEvent* Get(); 

    QEvent(); 

    qevent*      evt ; 
    qevent*      d_evt ; 

    const NP* gs ;  
    QBuf<float>* genstep ; 
    QBuf<int>*   seed  ;
    std::string  meta ; 

    void setGensteps(const NP* gs);
    void setGensteps(QBuf<float>* dgs ); 

    unsigned count_genstep_photons(); 


    void setMeta( const char* meta ); 
    bool hasMeta() const ; 

    void downloadPhoton( std::vector<quad4>& photon ); 
    void savePhoton( const char* dir, const char* name); 
    void saveGenstep(const char* dir, const char* name); 
    void saveMeta(   const char* dir, const char* name); 
 
    void checkEvt() ;  // GPU side 

    qevent* getDevicePtr() const ;
    unsigned getNumPhotons() const ;  
    std::string desc() const ; 
};




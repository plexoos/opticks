#pragma once
/**
Ctx : instanciation creates OptiX 7 optixDeviceContext and populates Properties  
==================================================================================

Instanciated by CSGOptiX::initCtx dump properties using: CSGOptiX=INFO


**/

#include <optix.h>
#include <vector>
#include <string>
#include "plog/Severity.h"

struct Properties ; 

struct Ctx 
{
    static const plog::Severity LEVEL ; 
    static OptixDeviceContext context ;
    static void log_cb( unsigned int level, const char* tag, const char* message, void* /*cbdata */); 

   
    Properties* props  ; 
    std::vector<std::string> log ; 

    Ctx(); 

    std::string desc() const ; 
    std::string loglines() const ; 

};


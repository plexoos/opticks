#pragma once

#include <vector>
#include <vector_types.h>
struct Params ; 

struct Frame
{
    Params* params ; 

    std::vector<float4> isect ; 
    std::vector<uchar4> pixels ; 
    uchar4* d_pixels = nullptr ; 
    float4* d_isect = nullptr ; 

    Frame(Params* params_);
 
    void init(); 
    void init_pixels();
    void init_isect();
 
    void download(); 
    void download_pixels();
    void download_isect();

    void writePPM(const char* path, bool yflip=true ); 
    void writeNPY(const char* path);

}; 



#include <optix_world.h>
#include <optixu/optixu_math_namespace.h>

using namespace optix;

rtDeclareVariable(uint2, launch_index, rtLaunchIndex, );
//rtDeclareVariable(uint2, launch_dim,   rtLaunchDim, );

rtBuffer<float4>  output_buffer;

RT_PROGRAM void writeBuffer()
{
    /*
    unsigned long long photon_id = launch_index.x ;  
    unsigned int photon_offset = photon_id*4 ; 
    rtPrintf("// minimal %llu \n", photon_id );
    */ 

    unsigned photon_id = launch_index.x ;  
    unsigned photon_offset = photon_id*4 ; 
 
    rtPrintf("// writeBufferTest.cu:writeBuffer %d \n", photon_id );
   
    output_buffer[photon_offset+0] = make_float4(40.f, 40.f, 40.f, 40.f);
    output_buffer[photon_offset+1] = make_float4(41.f, 41.f, 41.f, 41.f);
    output_buffer[photon_offset+2] = make_float4(42.f, 42.f, 42.f, 42.f);
    output_buffer[photon_offset+3] = make_float4(43.f, 43.f, 43.f, 43.f);

}

RT_PROGRAM void exception()
{
    rtPrintExceptionDetails();
}




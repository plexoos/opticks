#pragma once

#include "quad.h"

struct CerenkovStep
{
    int Id    ;
    int ParentId ;
    int MaterialIndex  ;
    int NumPhotons ;

    float3 x0 ;
    float  t0 ;

    float3 DeltaPosition ;
    float  step_length ;

    int code; 
    float charge ;
    float weight ;
    float MeanVelocity ; 

    float BetaInverse ; 
    float Pmin ; 
    float Pmax ; 
    float maxCos ; 
 
    float maxSin2 ;
    float MeanNumberOfPhotons1 ; 
    float MeanNumberOfPhotons2 ; 
    int   BialkaliMaterialIndex  ;

    // above are loaded parameters, below are derived from them
    float MeanNumberOfPhotonsMax ; 
    float3 p0 ;
    //
    // hmm whats the OptiX equivalent ? index pointer to approriate line of substance texture atlas 
    //Material* bialkaliMaterial ; 
    //Material* material ; 

};


__device__ void csload( CerenkovStep& cs, optix::buffer<float4>& cerenkov, unsigned int csid )
{

    int offset = 6*csid ; // 6 quads per item 

    union quad ipmn, ccwv, mmmm  ;
 
    ipmn.f = cerenkov[offset+0];     

    cs.Id = ipmn.i.x ; 
    cs.ParentId = ipmn.i.y ; 
    cs.MaterialIndex = ipmn.i.z ; 
    cs.NumPhotons = ipmn.i.w ; 

    float4 xt0 = cerenkov[offset+1];
    cs.x0 = make_float3(xt0.x, xt0.y, xt0.z );
    cs.t0 = xt0.w ; 
    
    float4 dpsl = cerenkov[offset+2] ;
    cs.DeltaPosition = make_float3(dpsl.x, dpsl.y, dpsl.z );
    cs.step_length = dpsl.w ; 

    ccwv.f = cerenkov[offset+3] ;
    cs.code = ccwv.i.x ;
    cs.charge = ccwv.f.y ;
    cs.weight = ccwv.f.z ;
    cs.MeanVelocity = ccwv.f.w ;


    float4 bppm = cerenkov[offset+4] ;
    cs.BetaInverse = bppm.x ; 
    cs.Pmin = bppm.y ; 
    cs.Pmax = bppm.z ; 
    cs.maxCos= bppm.w ; 

    mmmm.f = cerenkov[offset+5] ;
    cs.maxSin2 = mmmm.f.x ; 
    cs.MeanNumberOfPhotons1 = mmmm.f.y ; 
    cs.MeanNumberOfPhotons2 = mmmm.f.z ; 
    cs.BialkaliMaterialIndex = mmmm.i.w ; 
}


__device__ void csinit( CerenkovStep& cs /*, Geometry* g */)
{
    cs.p0 = normalize(cs.DeltaPosition);

    cs.MeanNumberOfPhotonsMax = max(cs.MeanNumberOfPhotons1, cs.MeanNumberOfPhotons2);

    //cs.material = g->materials[cs.MaterialIndex] ;
    //cs.bialkaliMaterial = g->materials[cs.BialkaliMaterialIndex] ;

}

__device__ void csdump( CerenkovStep& cs )
{
    rtPrintf("cs.Id %d ParentId %d MaterialIndex %d NumPhotons %d \n", 
       cs.Id, 
       cs.ParentId, 
       cs.MaterialIndex, 
       cs.NumPhotons 
       );

    rtPrintf("x0 %f %f %f  t0 %f \n", 
       cs.x0.x, 
       cs.x0.y, 
       cs.x0.z, 
       cs.t0 
       );

    rtPrintf("DeltaPosition %f %f %f  step_length %f  \n", 
       cs.DeltaPosition.x, 
       cs.DeltaPosition.y, 
       cs.DeltaPosition.z,
       cs.step_length
       ); 

    rtPrintf("code %d  charge %f weight %f MeanVelocity %f \n", 
       cs.code,
       cs.charge,
       cs.weight,
       cs.MeanVelocity
      );

    rtPrintf("BetaInverse %f  Pmin %f Pmax %f maxCos %f \n", 
       cs.BetaInverse,
       cs.Pmin,
       cs.Pmax,
       cs.maxCos
      );

    rtPrintf("maxSin2 %f  MeanNumberOfPhotons1 %f MeanNumberOfPhotons2 %f MeanNumberOfPhotonsMax %f \n", 
       cs.maxSin2,
       cs.MeanNumberOfPhotons1,
       cs.MeanNumberOfPhotons2,
       cs.MeanNumberOfPhotonsMax
      );

    rtPrintf("p0 %f %f %f  \n", 
       cs.p0.x, 
       cs.p0.y, 
       cs.p0.z
       );
}




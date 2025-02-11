/**
CSGOptiX7.cu 
===================

NB: ONLY CODE THAT MUST BE HERE DUE TO OPTIX DEPENDENCY SHOULD BE HERE
everything else should be located elsewhere : mostly in qudarap: sevent, qsim 
or the sysrap basis types sphoton quad4 quad2 etc.. where the code is reusable 
and more easily tested. 

Functions 
-----------

trace
    populate quad2 prd by call to optixTrace

make_color
    minimal normal "shader"

render
    raygen function : calling trace and "shading" pixels

simulate
    raygen function : qsim::generate_photon, bounce while loop, qsim::propagate

    * ifndef PRODUCTION sctx::trace sctx::point record the propagation point-by-point 

simtrace
    raygen function : qsim.h generate_photon_simtrace, trace, sevent::add_simtrace

__raygen__rg
    calls one of the above raygen functions depending on params.raygenmode 

setPayload
    mechanics of communication when not using WITH_PRD

__miss_ms
    default quad2 prd OR payload for rays that miss 

__closesthit__ch
    populate quad2 prd OR payload for rays that intersect    

__intersection__is
    converts OptiX HitGroupData into corresponding CSGNode and calls intersect_prim
    giving float4 isect: (normal_at_intersect, distance) 

**/

#include <optix.h>

#include "SRG.h"
#include "scuda.h"
#include "squad.h"
#include "sqat4.h"
#include "sphoton.h"
#include "scerenkov.h"
#include "sstate.h"

#ifndef PRODUCTION
#include "stag.h"
#include "sseq.h"
#include "srec.h"
#endif

#include "sevent.h"
#include "sctx.h"

// simulation 
#include <curand_kernel.h>

#include "qsim.h"

#include "csg_intersect_leaf.h"
#include "csg_intersect_node.h"
#include "csg_intersect_tree.h"

#include "Binding.h"
#include "Params.h"

#ifdef WITH_PRD
#include "Pointer.h"
#endif

extern "C" { __constant__ Params params ;  }

/**
trace : pure function, with no use of params, everything via args
-------------------------------------------------------------------

Outcome of trace is to populate *prd* by payload and attribute passing.
When WITH_PRD macro is defined only 2 32-bit payload values are used to 
pass the 64-bit  pointer, otherwise more payload and attributes values 
are used to pass the contents IS->CH->RG. 

See __closesthit__ch to see where the payload p0-p7 comes from.
**/

static __forceinline__ __device__ void trace(
        OptixTraversableHandle handle,
        float3                 ray_origin,
        float3                 ray_direction,
        float                  tmin,
        float                  tmax,
        quad2*                 prd
        )   
{
    const float rayTime = 0.0f ; 
    OptixVisibilityMask visibilityMask = 1u  ; 
    OptixRayFlags rayFlags = OPTIX_RAY_FLAG_DISABLE_ANYHIT ;   // OPTIX_RAY_FLAG_NONE 
    const unsigned SBToffset = 0u ; 
    const unsigned SBTstride = 1u ; 
    const unsigned missSBTIndex = 0u ; 
#ifdef WITH_PRD
    uint32_t p0, p1 ; 
    packPointer( prd, p0, p1 ); 
    optixTrace(
            handle,
            ray_origin,
            ray_direction,
            tmin,
            tmax,
            rayTime,
            visibilityMask,
            rayFlags,
            SBToffset,
            SBTstride,
            missSBTIndex,
            p0, p1
            );
#else
    uint32_t p0, p1, p2, p3, p4, p5, p6, p7  ; 
    optixTrace(
            handle,
            ray_origin,
            ray_direction,
            tmin,
            tmax,
            rayTime,
            visibilityMask,
            rayFlags,
            SBToffset,
            SBTstride,
            missSBTIndex,
            p0, p1, p2, p3, p4, p5, p6, p7
            );
    // unclear where the uint_as_float CUDA device function is defined, seems CUDA intrinsic without header ?
    prd->q0.f.x = uint_as_float( p0 );
    prd->q0.f.y = uint_as_float( p1 );
    prd->q0.f.z = uint_as_float( p2 );
    prd->q0.f.w = uint_as_float( p3 ); 
    prd->set_identity(p4) ; 
    prd->set_boundary(p5) ;  
    prd->set_lposcost(uint_as_float(p6)) ;  
    prd->set_iindex(p7) ;  
#endif
}

#if !defined(PRODUCTION) && defined(WITH_RENDER)

__forceinline__ __device__ uchar4 make_color( const float3& normal, unsigned identity, unsigned boundary )  // pure 
{
    // identity and boundary not used 
    float scale = 1.f ; 
    return make_uchar4(
            static_cast<uint8_t>( clamp( normal.x, 0.0f, 1.0f ) *255.0f )*scale ,
            static_cast<uint8_t>( clamp( normal.y, 0.0f, 1.0f ) *255.0f )*scale ,
            static_cast<uint8_t>( clamp( normal.z, 0.0f, 1.0f ) *255.0f )*scale ,
            255u
            );
}



/**
render : non-pure, uses params for viewpoint inputs and pixels output 
-----------------------------------------------------------------------

**/

static __forceinline__ __device__ void render( const uint3& idx, const uint3& dim, quad2* prd )
{
    float2 d = 2.0f * make_float2(
            static_cast<float>(idx.x)/static_cast<float>(dim.x),
            static_cast<float>(idx.y)/static_cast<float>(dim.y)
            ) - 1.0f;

    const bool yflip = true ;
    if(yflip) d.y = -d.y ;

    const unsigned cameratype = params.cameratype ;  
    const float3 dxyUV = d.x * params.U + d.y * params.V ; 
    const float3 origin    = cameratype == 0u ? params.eye                     : params.eye + dxyUV    ;
    const float3 direction = cameratype == 0u ? normalize( dxyUV + params.W )  : normalize( params.W ) ;
    //                           cameratype 0u:perspective,                    1u:orthographic

    trace( 
        params.handle,
        origin,
        direction,
        params.tmin,
        params.tmax,
        prd
    );

    const float3* normal = prd->normal();  
    float3 diddled_normal = normalize(*normal)*0.5f + 0.5f ; // diddling lightens the render, with mid-grey "pedestal" 
    unsigned index = idx.y * params.width + idx.x ;

    if(params.pixels) 
    {
        params.pixels[index] = make_color( diddled_normal, prd->identity(), prd->boundary() ); 
    }
    if(params.isect)  
    {
        float3 position = origin + direction*prd->distance() ;
        params.isect[index]  = make_float4( position.x, position.y, position.z, __uint_as_float(prd->identity())) ; 
    }
}
#endif
 

#if defined(WITH_SIMULATE)

/**
simulate : uses params for input: gensteps, seeds and output photons 
----------------------------------------------------------------------

Contrast with the monolithic old way with OptiXRap/cu/generate.cu:generate 

This method aims to get as much as possible of its functionality from 
separately implemented and tested headers. 

The big thing that CSGOptiX provides is geometry intersection, only that must be here. 
Everything else should be implemented and tested elsewhere, mostly in QUDARap headers.

Hence this "simulate" needs to act as a coordinator. 
Params take central role in enabling this:


Params
~~~~~~~

* CPU side params including qsim.h sevent.h pointers instanciated in CSGOptiX::CSGOptiX 
  and populated by CSGOptiX::init methods before being uploaded by CSGOptiX::prepareParam 

COMPARE WITH qsim::mock_propagate

**/

static __forceinline__ __device__ void simulate( const uint3& launch_idx, const uint3& dim, quad2* prd )
{
    sevent* evt = params.evt ; 
    if (launch_idx.x >= evt->num_photon) return;

    unsigned idx = launch_idx.x ;  // aka photon_idx
    unsigned genstep_idx = evt->seed[idx] ; 
    const quad6& gs = evt->genstep[genstep_idx] ; 
     
    qsim* sim = params.sim ; 
    curandState rng = sim->rngstate[idx] ;    // TODO: skipahead using an event_id 

    sctx ctx = {} ; 
    ctx.evt = evt ; 
    ctx.prd = prd ; 
    ctx.idx = idx ; 

    sim->generate_photon(ctx.p, rng, gs, idx, genstep_idx );  

    int command = START ; 
    int bounce = 0 ;  
#ifndef PRODUCTION
    ctx.point(bounce);  
#endif
    while( bounce < evt->max_bounce )
    {    
        trace( params.handle, ctx.p.pos, ctx.p.mom, params.tmin, params.tmax, prd);  // geo query filling prd      
        if( prd->boundary() == 0xffffu ) break ; // SHOULD ONLY HAPPEN FOR PHOTONS STARTING OUTSIDE WORLD
        // propagate can do nothing meaningful without a boundary 

        // HMM: normalize here or within CSG ? Actually only needed for 
        // geometry with active scaling, such as ellipsoid.  
        // TODO: move this so its only done when needed
        //     ~/o/notes/issues/CSGOptiX_simulate_avoid_normalizing_every_normal.rst
        //

        float3* normal = prd->normal();  
        *normal = normalize(*normal); 

#ifndef PRODUCTION
        ctx.trace(bounce);  
#endif
        command = sim->propagate(bounce, rng, ctx); 
        bounce++;     
#ifndef PRODUCTION
        ctx.point(bounce) ; 
#endif
        if(command == BREAK) break ;    
    }    
#ifndef PRODUCTION
    ctx.end();  // write seq, tag, flat 
#endif
    evt->photon[idx] = ctx.p ;
}

#endif


#if !defined(PRODUCTION) && defined(WITH_SIMTRACE)

/**
simtrace
----------

Used for making 2D cross section views of geometry intersects  

Note how seeding is still needed here despite the highly artificial 
nature of the center-extent grid of gensteps as the threads of the launch 
still needs to access different gensteps across the grid. 

TODO: Compose frames of pixels, isect and "fphoton" within the cegs window
using the positions of the intersect "photons".
Note that multiple threads may be writing to the same pixel 
that is apparently not a problem, just which does it is uncontrolled.

unsigned index = iz * params.width + ix ;
if( index > 0 )
{
    params.pixels[index] = make_uchar4( 255u, 0u, 0u, 255u) ;
    params.isect[index] = make_float4( ipos.x, ipos.y, ipos.z, uint_as_float(identity)) ; 
    params.fphoton[index] = p ; 
}
**/


static __forceinline__ __device__ void simtrace( const uint3& launch_idx, const uint3& dim, quad2* prd )
{
    unsigned idx = launch_idx.x ;  // aka photon_id
    sevent* evt  = params.evt ; 
    if (idx >= evt->num_simtrace) return;

    unsigned genstep_id = evt->seed[idx] ; 

#if defined(DEBUG_PIDX)
    if(idx == 0) printf("//CSGOptiX7.cu : simtrace idx %d genstep_id %d evt->num_simtrace %d \n", idx, genstep_id, evt->num_simtrace ); 
#endif

    const quad6& gs     = evt->genstep[genstep_id] ; 
     
    qsim* sim = params.sim ; 
    curandState rng = sim->rngstate[idx] ;   

    quad4 p ;  
    sim->generate_photon_simtrace(p, rng, gs, idx, genstep_id );  

    const float3& pos = (const float3&)p.q0.f  ; 
    const float3& mom = (const float3&)p.q1.f ; 

    trace( 
        params.handle,
        pos,
        mom,
        params.tmin,
        params.tmax,
        prd
    );

    evt->add_simtrace( idx, p, prd, params.tmin ); 
}
#endif

/**
for angular efficiency need intersection point in object frame to get the angles  
**/

extern "C" __global__ void __raygen__rg()
{
    const uint3 idx = optixGetLaunchIndex();
    const uint3 dim = optixGetLaunchDimensions();

    quad2 prd ; 
    prd.zero(); 
  
#ifndef PRODUCTION
    switch( params.raygenmode )
    {

#ifdef WITH_SIMULATE
        case SRG_SIMULATE:  simulate( idx, dim, &prd ) ; break ;  
#endif
#ifdef WITH_RENDER
        case SRG_RENDER:    render(   idx, dim, &prd ) ; break ;  
#endif
#ifdef WITH_SIMTRACE
        case SRG_SIMTRACE:  simtrace( idx, dim, &prd ) ; break ;  
#endif
    }
#else
    simulate( idx, dim, &prd ) ;
#endif
} 


#ifdef WITH_PRD
#else
/**
*setPayload* is used from __closesthit__ and __miss__ providing communication to __raygen__ optixTrace call
**/
static __forceinline__ __device__ void setPayload( float normal_x, float normal_y, float normal_z, float distance, unsigned identity, unsigned boundary, float lposcost, unsigned iindex )
{
    optixSetPayload_0( float_as_uint( normal_x ) );
    optixSetPayload_1( float_as_uint( normal_y ) );
    optixSetPayload_2( float_as_uint( normal_z ) );
    optixSetPayload_3( float_as_uint( distance ) );
    optixSetPayload_4( identity );
    optixSetPayload_5( boundary );
    optixSetPayload_6( lposcost );  
    optixSetPayload_7( iindex   );  

    // num_payload_values PIP::PIP must match the payload slots used up to maximum of 8 
    // NB : payload is distinct from attributes
}
#endif

/**
__miss__ms
-------------

* missing "normal" is somewhat render specific and this is used for 
  all raygenmode but Miss should never happen with real simulations 
* Miss can happen with simple geometry testing however when shoot 
  rays from outside the "world"

**/


extern "C" __global__ void __miss__ms()
{
    MissData* ms  = reinterpret_cast<MissData*>( optixGetSbtDataPointer() );
    const unsigned identity = 0xffffffffu ; 
    const unsigned boundary = 0xffffu ;
    const float lposcost = 0.f ; 
  
#ifdef WITH_PRD
    quad2* prd = getPRD<quad2>(); 

    prd->q0.f.x = ms->r ;   
    prd->q0.f.y = ms->g ; 
    prd->q0.f.z = ms->b ; 
    prd->q0.f.w = 0.f ; 

    prd->q1.u.x = 0u ; 
    prd->q1.u.y = 0u ; 
    prd->q1.u.z = 0u ; 
    prd->q1.u.w = 0u ; 

    prd->set_boundary(boundary); 
    prd->set_identity(identity); 
    prd->set_lposcost(lposcost); 
#else
    setPayload( ms->r, ms->g, ms->b, 0.f, identity, boundary, lposcost );  // communicate from ms->rg
#endif
}

/**
__closesthit__ch : pass attributes from __intersection__ into setPayload
============================================================================

optixGetInstanceId 
    flat instance_idx over all transforms in the single IAS, 
    JUNO maximum ~50,000 (fits within 0xffff = 65535)

optixGetPrimitiveIndex
    local index of AABB within the GAS, 
    instanced solids adds little to the number of AABB, 
    most come from unfortunate repeated usage of prims in the non-instanced global
    GAS with repeatIdx 0 (JUNO up to ~4000)

optixGetRayTmax
    In intersection and CH returns the current smallest reported hitT or the tmax passed into rtTrace 
    if no hit has been reported


**/

extern "C" __global__ void __closesthit__ch()
{
    unsigned iindex = optixGetInstanceIndex() ;    // 0-based index within IAS
    unsigned instance_id = optixGetInstanceId() ;  // user supplied instanceId, see IAS_Builder::Build 
    unsigned prim_idx = optixGetPrimitiveIndex() ; // GAS_Builder::MakeCustomPrimitivesBI_11N  (1+index-of-CSGPrim within CSGSolid/GAS)

    //unsigned identity = (( prim_idx & 0xffff ) << 16 ) | ( instance_id & 0xffff ) ; 
    unsigned identity = instance_id ;  // CHANGED July 2023, as now carrying sensor_identifier, see sysrap/sqat4.h 

#ifdef WITH_PRD
    quad2* prd = getPRD<quad2>(); 

    prd->set_identity( identity ) ;
    prd->set_iindex(   iindex ) ;
    float3* normal = prd->normal(); 
    *normal = optixTransformNormalFromObjectToWorldSpace( *normal ) ;  

#else
    const float3 local_normal =    // geometry object frame normal at intersection point 
        make_float3(
                uint_as_float( optixGetAttribute_0() ),
                uint_as_float( optixGetAttribute_1() ),
                uint_as_float( optixGetAttribute_2() )
                );

    const float distance = uint_as_float(  optixGetAttribute_3() ) ;  
    unsigned boundary = optixGetAttribute_4() ; 
    const float lposcost = uint_as_float( optixGetAttribute_5() ) ; 
    float3 normal = optixTransformNormalFromObjectToWorldSpace( local_normal ) ;  

    setPayload( normal.x, normal.y, normal.z, distance, identity, boundary, lposcost, iindex );  // communicate from ch->rg
#endif
}

/**
__intersection__is
----------------------

HitGroupData provides the numNode and nodeOffset of the intersected CSGPrim.
Which Prim gets intersected relies on the CSGPrim::setSbtIndexOffset

Note that optixReportIntersection returns a bool, but that is 
only relevant when using anyHit as it provides a way to ignore hits.
But Opticks does not use anyHit so the returned bool should 
always be true. 

The attributes passed into optixReportIntersection are 
available within the CH (and AH) programs. 

**/

extern "C" __global__ void __intersection__is()
{
    HitGroupData* hg  = (HitGroupData*)optixGetSbtDataPointer();  
    int nodeOffset = hg->nodeOffset ; 

    const CSGNode* node = params.node + nodeOffset ;  // root of tree
    const float4* plan = params.plan ;  
    const qat4*   itra = params.itra ;  

    const float  t_min = optixGetRayTmin() ; 
    const float3 ray_origin = optixGetObjectRayOrigin();
    const float3 ray_direction = optixGetObjectRayDirection();

    float4 isect ; // .xyz normal .w distance 
    if(intersect_prim(isect, node, plan, itra, t_min , ray_origin, ray_direction ))  
    {
        const float lposcost = normalize_z(ray_origin + isect.w*ray_direction ) ;  // scuda.h 
        const unsigned hitKind = 0u ;            // only 8bit : could use to customize how attributes interpreted
        const unsigned boundary = node->boundary() ;  // all nodes of tree have same boundary 

#ifdef WITH_PRD
        if(optixReportIntersection( isect.w, hitKind))
        {
            quad2* prd = getPRD<quad2>(); 
            prd->q0.f = isect ;  // .w:distance and .xyz:normal which starts as the local frame one 
            prd->set_boundary(boundary) ; 
            prd->set_lposcost(lposcost); 
        }   
#else
        unsigned a0, a1, a2, a3, a4, a5  ; // MUST CORRESPOND TO num_attribute_values in PIP::PIP 
        a0 = float_as_uint( isect.x );     // isect.xyz is object frame normal of geometry at intersection point 
        a1 = float_as_uint( isect.y );
        a2 = float_as_uint( isect.z );
        a3 = float_as_uint( isect.w ) ; 
        a4 = boundary ; 
        a5 = float_as_uint( lposcost ); 
        optixReportIntersection( isect.w, hitKind, a0, a1, a2, a3, a4, a5 );   
#endif
        // IS:optixReportIntersection writes the attributes that can be read in CH and AH programs 
        // max 8 attribute registers, see PIP::PIP, communicate to __closesthit__ch 
    }
}
// story begins with intersection

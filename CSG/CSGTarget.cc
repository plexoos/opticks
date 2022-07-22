#include <csignal>

#include "PLOG.hh"
#include "scuda.h"
#include "sqat4.h"
#include "sframe.h"
#include "SCenterExtentFrame.h"

#include "CSGFoundry.h"
#include "CSGTarget.h"

const plog::Severity CSGTarget::LEVEL = PLOG::EnvLevel("CSGTarget", "DEBUG" ); 

CSGTarget::CSGTarget( const CSGFoundry* foundry_ )
    :
    foundry(foundry_)
{
}

/**
CSGTarget::getCenterExtent
----------------------------

Used by CSGFoundry::getCenterExtent

*ce*
    center-extent float4 set when return code is zero
*midx*
    solid (aka mesh) index : identify the shape
*mord*
    solid (aka mesh) ordinal : pick between shapes when there are more than one, used with global(non-instanced) geometry 
*iidx*
    instance index, >-1 for global instance, -1 for local non-instanced 



2022-01-03 15:06:16.746 INFO  [5325503] [CSGTarget::getLocalCenterExtent@91]  midx 88 mord 10 prim.size 64 mord_in_range 1
2022-01-03 15:06:16.746 INFO  [5325503] [CSGTarget::getLocalCenterExtent@109]  lce (-17336.020,-4160.728,-809.117,66.045) 


-2:world2model_rtpw : scale down and rotate    ( world2model = irotate * iscale * itranslate ) 

 MOI solidXJfixture:10:-2 midx 88 mord 10 iidx -2 gce (-17336.020,-4160.728,-809.117,66.045) 
 q (-0.015, 0.001, 0.004, 0.000) (-0.004, 0.000,-0.015, 0.000) (-0.001,-0.015, 0.000, 0.000) (-270.221,-0.000, 0.000, 1.000) 

-3:model2world_rtpw : scale up and rotate      ( model2world = translate * scale * rotate )

 MOI solidXJfixture:10:-3 midx 88 mord 10 iidx -3 gce (-17336.020,-4160.728,-809.117,66.045) 
 q (-64.155,-15.397,-2.994, 0.000) ( 2.912, 0.699,-65.977, 0.000) (15.413,-64.221, 0.000, 0.000) (-17336.020,-4160.728,-809.117, 1.000) 

-4:world2model_xyzw : uniform scaling down only, no rotation

 MOI solidXJfixture:10:-4 midx 88 mord 10 iidx -4 gce (-17336.020,-4160.728,-809.117,66.045) 
 q ( 0.015, 0.000, 0.000, 0.000) ( 0.000, 0.015, 0.000, 0.000) ( 0.000, 0.000, 0.015, 0.000) (262.489,62.999,12.251, 1.000) 

-5:model2world_xyzw  : uniform scaling up only, no rotation

 MOI solidXJfixture:10:-5 midx 88 mord 10 iidx -5 gce (-17336.020,-4160.728,-809.117,66.045) 
 q (66.045, 0.000, 0.000, 0.000) ( 0.000,66.045, 0.000, 0.000) ( 0.000, 0.000,66.045, 0.000) (-17336.020,-4160.728,-809.117, 1.000) 


**/

int CSGTarget::getCenterExtent(float4& ce, int midx, int mord, int iidxg, qat4* m2w, qat4* w2m ) const 
{
    LOG(LEVEL) << " (midx mord iidxg) " << "(" << midx << " " << mord << " " << iidxg << ") " ;  
    if( iidxg == -1 )
    {
        // HMM: CSGFoundry::getCenterExtent BRANCHES FOR iidxg == -1 SO THIS WILL NOT BE CALLED 
        // HMM: maybe not that branch is for midx -1 ? 

        LOG(info) << "(iidxg == -1) qptr transform will not be set, typically defaulting to identity " ; 
        int lrc = getLocalCenterExtent(ce, midx, mord); 
        if(lrc != 0) return 1 ; 
    }
    else if( iidxg == -2 || iidxg == -3 )
    {
        LOG(info) << "(iidxg == -2/-3  EXPERIMENTAL qptr transform will be set to SCenterExtentFrame transforms " ; 
        int lrc = getLocalCenterExtent(ce, midx, mord); 
        if(lrc != 0) return 1 ; 

        if( iidxg == -2 )
        {
            SCenterExtentFrame<double> cef_xyzw( ce.x, ce.y, ce.z, ce.w, false );  
            m2w->read_narrow(cef_xyzw.model2world_data); 
            w2m->read_narrow(cef_xyzw.world2model_data); 
        }
        else if( iidxg == -3 )
        {
            SCenterExtentFrame<double> cef_rtpw( ce.x, ce.y, ce.z, ce.w, true );  
            m2w->read_narrow(cef_rtpw.model2world_data); 
            w2m->read_narrow(cef_rtpw.world2model_data);  
        }
    }
    else
    {
        int grc = getGlobalCenterExtent(ce, midx, mord, iidxg, m2w, w2m ); 
        // TODO: paired transforms also ?
        //  HMM: the m2w here populated is from the (midx, mord, iidxg) instance transform, with identity info 
        if(grc != 0) return 2 ;
    }
    return 0 ; 
}


/**
CSGTarget::getFrame
----------------------

Q: is indexing by MOI and inst_idx equivalent ? OR: Can a MOI be converted into inst_idx and vice versa ?
A: see notes with CSGFoundry::getFrame

**/

int CSGTarget::getFrame(sframe& fr,  int midx, int mord, int iidxg ) const 
{
    fr.set_midx_mord_iidx( midx, mord, iidxg ); 
    return getCenterExtent( fr.ce, midx, mord, iidxg, &fr.m2w , &fr.w2m ); 
}

int CSGTarget::getFrame(sframe& fr, int inst_idx ) const 
{
    const qat4* _t = foundry->getInst(inst_idx); 

    unsigned ins_idx, gas_idx, ias_idx ; 
    _t->getIdentity(ins_idx, gas_idx, ias_idx )  ;

    assert( int(ins_idx) == inst_idx ); 
    fr.set_inst(inst_idx); 
   
    // HMM: these values are already there inside the matrices ? 
    fr.set_ins_gas_ias(ins_idx, gas_idx, ias_idx ) ; 


    qat4 t(_t->cdata());   // copy the instance (transform and identity info)
    const qat4* v = Tran<double>::Invert(&t);     // identity gets cleared in here 

    qat4::copy(fr.m2w,  t);  
    qat4::copy(fr.w2m, *v);  

    const CSGSolid* solid = foundry->getSolid(gas_idx); 
    fr.ce = solid->center_extent ;  

    // although there can be multiple CSGPrim within the CSGSolid
    // there is not way from the inst_idx to tell which one is needed
    // so use the CSGSolid one as that should combined the ce of all the CSGPrim

    return 0 ; 
}




/**
CSGTarget::getLocalCenterExtent : finds the (midx,mord) CSGPrim and returns its CE
-------------------------------------------------------------------------------------

Collects prim matching the *midx* and selects the *mord* ordinal one
from which to read the localCE 

**/

int CSGTarget::getLocalCenterExtent(float4& lce, int midx, int mord) const 
{
    std::vector<CSGPrim> prim ; 
    foundry->getMeshPrimCopies(prim, midx );  
    bool mord_in_range = mord < int(prim.size()) ; 

    LOG(info)  
        << " midx " << midx
        << " mord " << mord 
        << " prim.size " << prim.size()
        << " mord_in_range " << mord_in_range
        ;   

    if(!mord_in_range) return 1 ; 

    const CSGPrim& lpr = prim[mord] ;   

    float4 localCE = lpr.ce(); 

    lce.x = localCE.x ; 
    lce.y = localCE.y ; 
    lce.z = localCE.z ; 
    lce.w = localCE.w ; 

    LOG(info) << " lce " << lce  ;   
    return 0 ; 
}



/**
CSGTarget::getGlobalCenterExtent
---------------------------------

1. first find the MORD-inal prim *lpr* which has MIDX for its midx

   * midx corresponds to GGeo lvIdx or soIdx

2. use the prim to lookup indices for the solid(gas_idx) and prim 
3. collect instance transforms matching the *gas_idx*
4. select the *iidx* instance transform to construct a global-prim *gpr* 
5. fill in *gce* with the global center-extren from  

*gce*
    output global center extent float4
*midx* 
    input mesh index (aka lv index) 
*mord*
    input mesh ordinal : this is particularly useful with the global geometry where there are 
    no instances to select between. But there are repeated uses of the mesh that 
    this ordinal picks between. For instanced geometry this will mostly be zero(?)
*iidxg*
    input instance index, for example this could select a particular PMT 

    * NB this is not the global instance index, it is the GAS ordinael 

*qptr*
    output instance transform pointer. When non-null the instance
    transform will be copied into this qat4 which will contain 
    identity integers in its fourth column 


TODO: check this with global non-instanced geometry 

**/

int CSGTarget::getGlobalCenterExtent(float4& gce, int midx, int mord, int iidx, qat4* m2w, qat4* w2m ) const 
{
    const qat4* t = getInstanceTransform(midx, mord, iidx); 
    if(t == nullptr)
    {
        LOG(fatal) 
            << " failed to get InstanceTransform (midx mord iidx) " 
            << "(" << midx << " " << mord << " " << iidx << ")" 
            ;
        return 1 ;  
    }

    const qat4* v = Tran<double>::Invert(t );    
    // TODO: avoid this Invert by collecting paired instance transforms from Geant4 source 

    if(m2w) qat4::copy(*m2w, *t);  
    if(w2m) qat4::copy(*w2m, *v);  


    qat4 q(t->cdata());   // copy the instance (transform and identity info)

    unsigned ins_idx, gas_idx, ias_idx ; 
    q.getIdentity(ins_idx, gas_idx, ias_idx )  ;
    q.clearIdentity();           // clear before doing any transforming 


    const CSGPrim* lpr = foundry->getMeshPrim(midx, mord);  

    CSGPrim gpr = {} ; 
    CSGPrim::Copy(gpr, *lpr);   // start global prim from local 

    q.transform_aabb_inplace( gpr.AABB_() ); 

    LOG(LEVEL) 
        << " q " << q 
        << " ins_idx " << ins_idx
        << " ias_idx " << ias_idx
        ; 
    float4 globalCE = gpr.ce(); 
    gce.x = globalCE.x ; 
    gce.y = globalCE.y ; 
    gce.z = globalCE.z ; 
    gce.w = globalCE.w ; 

    LOG(LEVEL) 
        << " gpr " << gpr.desc()
        << " gce " << gce 
        ; 

    return 0 ; 
}


/**
CSGTarget::getTransform TODO eliminate this switching instead to getInstanceTransform
----------------------------------------------------------------------------------------

**/

int CSGTarget::getTransform(qat4& q, int midx, int mord, int iidx) const 
{
    const qat4* qi = getInstanceTransform(midx, mord, iidx); 
    if( qi == nullptr )
    {
        return 1 ; 
    }
    qat4::copy(q, *qi); 
    return 0 ; 
}

/**
CSGTarget::getInstanceTransform
---------------------------------

1. *getMeshPrim* finds the (midx, mord) CSGPrim which gives the repeatIdx (aka:gas_idx or compound solid index) 
2. *getInstanceGAS* finds the (gas_idx, iidx) instance transform 

This method avoids duplication between CSGTarget::getTransform and  CSGTarget::getGlobalCenterExtent 

**/

const qat4* CSGTarget::getInstanceTransform(int midx, int mord, int iidx) const 
{
    const CSGPrim* lpr = foundry->getMeshPrim(midx, mord);  
    if(!lpr)
    {
        LOG(fatal) << "Foundry::getMeshPrim failed for (midx mord) " << "(" << midx << " " <<  mord << ")"  ; 
        return nullptr ; 
    }

    const float4 local_ce = lpr->ce() ; 
    unsigned repeatIdx = lpr->repeatIdx(); // use the prim to lookup indices for  the solid and prim 
    unsigned primIdx = lpr->primIdx(); 
    unsigned gas_idx = repeatIdx ; 

    LOG(LEVEL) 
        << " (midx mord iidx) " << "(" << midx << " " << mord << " " << iidx << ") "
        << " lpr " << lpr
        << " repeatIdx " << repeatIdx
        << " primIdx " << primIdx
        << " local_ce " << local_ce 
        ; 

    const qat4* qi = foundry->getInstanceGAS(gas_idx, iidx ); 
    return qi ; 
}







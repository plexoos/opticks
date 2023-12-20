#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "plog/Severity.h"

struct SBitSet ; 
struct NP ; 
struct SSim ; 
struct stree ; 

#include "scuda.h"
#include "squad.h"
#include "sqat4.h"
#include "saabb.h"
#include "stran.h"
#include "SGeo.hh"

struct sframe ; 
struct SName ; 
struct CSGTarget ; 
struct CSGMaker ; 
struct CSGImport ; 

#include "CSGEnum.h"
#include "CSGSolid.h"
#include "CSGPrim.h"
#include "CSGNode.h"


#include "CSG_API_EXPORT.hh"

/**
CSGFoundry
============

::

        +-------+     +-------+    +---------+     +--------+       +-------+   +-------+        +------+
        | inst  |     | solid |    |  prim   |     | node   |       | tran  |   | itra  |        | plan |
        +=======+     +=======+    +=========+     +========+       +=======+   +=======+        +======+
        | s=0   | ->  |       | -> |         | ->  |        |  ->   |       |   |       |        |      |
        | s=0   |     |       |    |         |     |        |       |       |   |       |        |      |
        | s=0   |     |       |    +---------+     |        |       |       |   |       |        |      |
        | s=0   |     +-------+    |         |     |        |       |       |   |       |        +------+
        | s=1   |                  |         |     |        |       |       |   |       |
        | s=1   |                  |         |     +--------+       |       |   |       |
        | s=1   |                  |         |     |        |       |       |   |       |
        | s=1   |                  +---------+     |        |       |       |   |       |
        +-------+                                  |        |       |       |   |       |
                                                   |        |       |       |   |       |
                                                   |        |       |       |   |       |
                                                   |        |       |       |   |       |
                                                   |        |       |       |   |       |
                                                   |        |       |       |   |       |
                                                   +--------+       |       |   |       |  
                                                                    |       |   |       |
                                                                    |       |   |       |
                                                                    |       |   |       |
                                                                    |       |   |       |
                                                                    +-------+   +-------+
       
inst 
   array of qat4 with single solid references (gas_idx)

solid
   references range of prim with (numPrim, primOffset)  
prim
   references range of node with (numNode, nodeOffset) (roughly corresponds to G4VSolid)
node
   references single transform with tranOffset 
   sometimes references range of planes with (planOffset, numPlan)
   (CSG constituent nodes : sphere, box, boolean operators)

tran
    array of qat4 node transforms, same size as itra
itra
    array of qat4 node inverse transforms, same size as tran
plan 
    array of float4 planes 


**/


struct CSG_API CSGFoundry : public SGeo
{
    static const plog::Severity LEVEL ; 
    static const int  VERBOSE ; 
    static const unsigned IMAX ; 
    static const char* BASE ; 
    static const char* RELDIR ; 
    static const constexpr unsigned UNDEFINED = ~0u ; 

    // MOVED TO CSGMaker
    //static CSGFoundry* MakeGeom(const char* geom);
    //static CSGFoundry* MakeDemo(); 
    //static CSGFoundry* LoadGeom(const char* geom=nullptr); 


    const char* descELV() const ; 
    static const char* ELVString(const SName* id); 
    static const SBitSet* ELV(const SName* id); 

    static bool Load_saveAlt ; 
    static CSGFoundry* CreateFromSim();
    static CSGFoundry* Load();
    static CSGFoundry* CopySelect(const CSGFoundry* src, const SBitSet* elv ); 

    static const char* ResolveCFBase_(); 
    static const char* ResolveCFBase(); 
    static CSGFoundry* Load_();
    static CSGFoundry* Load(const char* base, const char* rel=RELDIR );

    void setOverrideSim( const SSim* ssim ); 
    const SSim* getSim() const ; 

    // TODO: farm off comparison machinery into another struct 

    static int Compare(const CSGFoundry* a , const CSGFoundry* b ); 
    static int WIP_CompareStruct(const CSGFoundry* a , const CSGFoundry* b ); 
    static std::string DescCompare( const CSGFoundry* a, const CSGFoundry* b ); 

    template<typename T>
    static int CompareVec( const char* name, const std::vector<T>& a, const std::vector<T>& b );

    template<typename T>
    static int CompareStruct( const char* name, const std::vector<T>& a, const std::vector<T>& b );

    static int CompareFloat4( const char* name, const std::vector<float4>& aa, const std::vector<float4>& bb ); 
    static bool Float4_IsDiff( const float4& a , const float4& b ); 


    static int CompareBytes(const void* a, const void* b, unsigned num_bytes);

    static CSGFoundry* INSTANCE ; 
    static CSGFoundry* Get() ; 

    CSGFoundry();
    void init(); 

    const char* getFold() const ;
    void setFold(const char* fold); 
    void setGeom(const char* geom); 
    void setOrigin(const CSGFoundry* origin); 
    void setElv(const SBitSet* elv); 

    std::string brief() const ;
    std::string desc() const ;
    std::string descBase() const ;
    std::string descComp() const ;
    std::string descSolid() const ; 
    std::string descMeshName() const ; 
    std::string descGAS() const ;

    void summary(const char* msg="CSGFoundry::summary") const ;
    std::string descSolids() const ;
    std::string descInst(unsigned ias_idx_, unsigned long long emm=~0ull ) const ;

    std::string descInstance() const ;   // IDX=0,10,100,1000,10000
    std::string descInstance(unsigned idx) const ; 

    void dump() const ;
    void dumpSolid() const ;
    void dumpSolid(unsigned solidIdx ) const ;
    int findSolidIdx(const char* label) const  ; // -1 if not found
    void findSolidIdx(std::vector<unsigned>& solid_idx, const char* label) const ; 
    std::string descSolidIdx( const std::vector<unsigned>& solid_selection ) ; 

    void dumpPrim() const ;
    void dumpPrim(unsigned solidIdx ) const ;
    std::string descPrim() const ;
    std::string descPrim(unsigned solidIdx) const  ;

    int getPrimBoundary(unsigned primIdx) const ; 
    void setPrimBoundary(unsigned primIdx, unsigned boundary) ; 

    std::string detailPrim() const ; 
    std::string detailPrim(unsigned primIdx) const ; 

    std::string descPrimSpec() const ; 
    std::string descPrimSpec(unsigned solidIdx) const ; 


    void dumpNode() const ;
    void dumpNode(unsigned solidIdx ) const ;
    std::string descNode() const ;
    std::string descNode(unsigned solidIdx) const  ;
    std::string descTran(unsigned solidIdx) const  ; 


    AABB iasBB(unsigned ias_idx_, unsigned long long emm=0ull ) const ;
    float4 iasCE(unsigned ias_idx_, unsigned long long emm=0ull ) const;
    void   iasCE(float4& ce, unsigned ias_idx_, unsigned long long emm=0ull ) const;
    void   gasCE(float4& ce, unsigned gas_idx ) const ;
    void   gasCE(float4& ce, const std::vector<unsigned>& gas_idxs ) const ; 

    float  getMaxExtent(const std::vector<unsigned>& solid_selection) const ;
    std::string descSolids(const std::vector<unsigned>& solid_selection) const ;


    CSGPrimSpec getPrimSpec(       unsigned solidIdx) const ;
    CSGPrimSpec getPrimSpecHost(   unsigned solidIdx) const ;
    CSGPrimSpec getPrimSpecDevice( unsigned solidIdx) const ;
    void        checkPrimSpec(     unsigned solidIdx) const ;
    void        checkPrimSpec() const ;


    const CSGSolid*   getSolidByName(const char* name) const ;
    const CSGSolid*   getSolid_(int solidIdx) const ;   // -ve counts from back 
    unsigned          getSolidIdx(const CSGSolid* so) const ; 


    unsigned getNumSolid(int type_) const ;
    unsigned getNumSolid() const;        // STANDARD_SOLID count 
    unsigned getNumSolidTotal() const;   // all solid count 

    unsigned getNumPrim() const;   
    unsigned getNumNode() const;
    unsigned getNumPlan() const;
    unsigned getNumTran() const;
    unsigned getNumItra() const;
    unsigned getNumInst() const;

    const CSGSolid*   getSolid(unsigned solidIdx) const ;  
    const CSGPrim*    getPrim(unsigned primIdx) const ;    
    const CSGNode*    getNode(unsigned nodeIdx) const ;
    const float4*     getPlan(unsigned planIdx) const ;
    const qat4*       getTran(unsigned tranIdx) const ;
    const qat4*       getItra(unsigned itraIdx) const ;
    const qat4*       getInst(unsigned instIdx) const ;

    CSGNode*          getNode_(unsigned nodeIdx);


    void              getNodePlanes(std::vector<float4>& planes, const CSGNode* nd) const ;  
    const CSGPrim*    getSolidPrim(unsigned solidIdx, unsigned primIdxRel) const ;
    const CSGNode*    getSolidPrimNode(unsigned solidIdx, unsigned primIdxRel, unsigned nodeIdxRel) const ;

    void getMeshPrimCopies(  std::vector<CSGPrim>& select_prim, unsigned mesh_idx ) const ;
    void getMeshPrimPointers(std::vector<const CSGPrim*>& select_prim, unsigned mesh_idx ) const ; 
    const CSGPrim* getMeshPrim( unsigned midx, unsigned mord ) const ; 

    unsigned getNumMeshPrim(unsigned mesh_idx ) const ;
    std::string descMeshPrim() const ;  

    unsigned getNumSelectedPrimInSolid(const CSGSolid* solid, const SBitSet* elv ) const ; 


    CSGSolid* addSolid(unsigned num_prim, const char* label, int primOffset_ = -1 );

    int       addPrim_solidLocalPrimIdx() const ; 
    CSGPrim*  addPrim(int num_node, int nodeOffset_ = -1 );

    int       addNode_solidLocalNodeIdx() const ; 
    CSGNode*  addNode(CSGNode nd); 
    CSGNode*  addNode();
    CSGNode*  addNode(CSGNode nd, const std::vector<float4>* pl, const Tran<double>* tr );
    CSGNode*  addNodes(const std::vector<CSGNode>& nds );

    CSGNode*  addNode(AABB& bb, CSGNode nd );
    CSGNode*  addNodes(AABB& bb, std::vector<CSGNode>& nds, const std::vector<const Tran<double>*>* trs  ); 

    CSGPrim*  addPrimNodes(AABB& bb, const std::vector<CSGNode>& nds, const std::vector<const Tran<double>*>* trs=nullptr ); 


    float4*   addPlan(const float4& pl );


    CSGSolid* addDeepCopySolid(unsigned solidIdx, const char* label=nullptr );


    template<typename T> unsigned addTran( const Tran<T>* tr  );
    template<typename T> unsigned addTran_( const Tran<T>* tr  );
    unsigned addTran( const qat4* tr, const qat4* it ) ;
    unsigned addTran() ;
    void     addTranPlaceholder(); 

    // adds transform and associates it with the node
    template<typename T> const qat4* addNodeTran(CSGNode* nd, const Tran<T>* tr, bool transform_node_aabb  ); 
    void addNodeTran(CSGNode* nd ); 


    void addInstance(const float* tr16, int gas_idx, int sensor_identifier, int sensor_index, bool firstcall = false ); 
    void addInstanceVector(const std::vector<glm::tmat4x4<float>>& v_inst_f4 ); 
    void addInstancePlaceholder(); 


    // via maker
    void makeDemoSolids();  
    void importSim(); 


    CSGSolid* make(const char* name); 


    static void DumpAABB(const char* msg, const float* aabb); 


    const char* getBaseDir(bool create) const ; 

    void save_(const char* dir) const ;
    void save(const char* base, const char* rel=nullptr ) const ;
    void saveAlt() const ; 

    // these argumentless methods require CFBASE envvar or geom member to be set 
    void save() const ; 
    void load() ; 

    static const char* load_FAIL_base_null_NOTES ; 
    static const char* LoadFailNotes(); 

    void load( const char* base, const char* rel ) ; 
    void setCFBase( const char* cfbase_ ); 
    const char* getCFBase() const ; 
    const char* getOriginCFBase() const ; 

    static const char* LOAD_FAIL_NOTES ; 
    void load( const char* dir ) ; 
    NP* loadAux(const char* auxrel="Values/values.npy" ) const ; 

    static int MTime(const char* dir, const char* fname_); 


    template<typename T> void loadArray( std::vector<T>& vec, const char* dir, const char* name, bool optional=false ); 

    void upload();
    bool isUploaded() const ; 

    void inst_find_unique(); 
    unsigned getNumUniqueGAS() const ;
    unsigned getNumUniqueIAS() const ;
    /*
    unsigned getNumUniqueINS() const ;
    */

    unsigned getNumInstancesIAS(int ias_idx, unsigned long long emm) const ;
    void     getInstanceTransformsIAS(std::vector<qat4>& select_inst, int ias_idx, unsigned long long emm ) const ;

    unsigned getNumInstancesGAS(int gas_idx) const ;
    void     getInstanceTransformsGAS(std::vector<qat4>&  select_qv, int gas_idx ) const ;  // collecting by value : TODO eliminate, swiching to getInstancePointersGAS
    void     getInstancePointersGAS(  std::vector<const qat4*>& select_qi, int gas_idx ) const ;  // collecting pointers to the actual instances 

    int       getInstanceIndex(int gas_idx_ , unsigned ordinal) const ; 
    const qat4* getInstance_with_GAS_ordinal(int gas_idx_ , unsigned ordinal=0) const  ;

    // id 
    void parseMOI(int& midx, int& mord, int& iidx, const char* moi) const ; 
    const char* getName(unsigned midx) const ;  

    static const char* getFrame_NOTES ; 
    sframe getFrame() const ; 
    sframe getFrame(const char* moi_or_iidx) const ; 

    int getFrame(sframe& fr, const char* frs ) const ; 
    int getFrame(sframe& fr, int midx, int mord, int gord) const ; 
    int getFrame(sframe& fr, int ins_idx ) const ; 

    sframe getFrameE() const ; 
    static void AfterLoadOrCreate(); 


    // target  
    int getCenterExtent(float4& ce, int midx, int mord, int gord=-1, qat4* m2w=nullptr, qat4* w2m=nullptr ) const ;
    int getTransform(   qat4& q   , int midx, int mord, int gord=-1) const ;

    template <typename T> void setMeta( const char* key, T value ); 
    template <typename T> T    getMeta( const char* key, T fallback); 
    bool hasMeta() const ; 

    void kludgeScalePrimBBox( const char* label, float dscale );
    void kludgeScalePrimBBox( unsigned solidIdx, float dscale );

    unsigned getNumMeshName() const ; 
    unsigned getNumSolidLabel() const ; 

    static void CopyNames(    CSGFoundry* dst, const CSGFoundry* src ); 
    static void CopyMeshName( CSGFoundry* dst, const CSGFoundry* src ); 

    void getMeshName( std::vector<std::string>& mname ) const ; 
    void getPrimName( std::vector<std::string>& pname ) const ; 

    // SGeo protocol 
    unsigned getNumMeshes() const ; 
    const char* getMeshName(unsigned midx) const ; 
    int getMeshIndexWithName(const char* name, bool startswith) const ; 
    int lookup_mtline(int mtindex) const ; 
    std::string desc_mt() const ; 



    int findMeshIndex(const char* qname) const ; 

    const std::string descELV(const SBitSet* elv) const ; 

    const std::string& getSolidLabel(unsigned sidx) const ; 

    void addMeshName(const char* name); 
    void addSolidLabel(const char* label); 

    // MEMBERS


    // HMM "meshname" is historical, should be "lvname" or "soname" ?
    std::vector<std::string> meshname ;  // GGeo::getMeshNames/GMeshLib (G4VSolid names from Geant4) should be primName in CF model ?
    std::vector<std::string> mmlabel ;   // from GGeo::getMergedMeshLabels eg of form "3084:sWorld" "7:HamamatsuR12860sMask_virtual"

    std::vector<CSGSolid>  solid ;   
    std::vector<CSGPrim>   prim ; 
    std::vector<CSGNode>   node ; 
    std::vector<float4>    plan ; 
    std::vector<qat4>      tran ;  
    std::vector<qat4>      itra ;  
    std::vector<qat4>      inst ;  


    CSGPrim*    d_prim ; 
    CSGNode*    d_node ; 
    float4*     d_plan ; 
    qat4*       d_itra ; 


    std::vector<int> gas ;


    const SSim* sim ; 
    CSGImport*  import ; 
    SName*    id ;   // holds the meshname vector of G4VSolid names 

    CSGTarget*  target ; 
    CSGMaker*   maker ; 
    bool        deepcopy_everynode_transform ; 

    CSGSolid*   last_added_solid ; 
    CSGPrim*    last_added_prim ; 
    CSGNode*    last_added_node ; 


    void setPrimBoundary(unsigned primIdx, const char* bname) ; 
    int  mtime ; 

    std::string meta ; 
    const char* fold ; 
    const char* cfbase ; 
    const char* geom ; 
    const char* loaddir ; 

    const CSGFoundry* origin ; 
    const SBitSet*    elv ; 
};


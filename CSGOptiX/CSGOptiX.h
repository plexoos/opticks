#pragma once

#include <vector>
#include <string>
#include <glm/fwd.hpp>

#include <optix.h>

#include "plog/Severity.h"
#include "sframe.h"

#include "CSGOPTIX_API_EXPORT.hh"

struct SMeta ; 
struct NP ; 
struct quad4 ; 
struct quad6 ; 
struct qat4 ; 
struct float4 ; 

struct scontext ; 
struct SGLM ; 
struct SSim ; 

struct CSGFoundry ; 
struct CSGView ; 

template <typename T> struct Tran ; 
struct QSim ; 
struct QEvent ; 

struct Params ; 
class Opticks ; 
class Composition ; 

#if OPTIX_VERSION < 70000
struct Six ; 
struct Dummy ; 
#else
struct Ctx ; 
struct PIP ; 
struct SBT ; 
#endif
struct Frame ; 

#include "SCSGOptiX.h"

struct CSGOPTIX_API CSGOptiX : public SCSGOptiX
{
    friend struct QSim ; 

    static constexpr const char* RELDIR = "CSGOptiX" ;  
    static const plog::Severity LEVEL ; 
    static CSGOptiX*   INSTANCE ; 
    static CSGOptiX*   Get(); 
    static int         Version(); 

    static int         RenderMain();    // used by tests/CSGOptiXRdrTest.cc 
    static int         SimtraceMain();  // used by tests/CSGOptiXTMTest.cc
    static int         SimulateMain();  // used by tests/CSGOptiXSMTest.cc 
    static int         Main();          // NOT USED

    static const char* Desc(); 

    static const char* TOP ; 
    static const char* PTXNAME ; 
    static const char* GEO_PTXNAME ; 

    SGLM*             sglm ; 

    const char*       flight ; 
    const CSGFoundry* foundry ; 
    const char*       prefix ; 
    const char*       outdir ; 
    const char*       cmaketarget ; 
    const char*       ptxpath ; 
    const char*       geoptxpath ; 
    float             tmin_model ; 
    int               jpg_quality ; 

    std::vector<unsigned>  solid_selection ;
    std::vector<double>  launch_times ;

    int               raygenmode ; 
    Params*           params  ; 


#if OPTIX_VERSION < 70000
    Six* six ;  
    Dummy* dummy0 ; 
    Dummy* dummy1 ; 
#else
    Ctx* ctx ; 
    PIP* pip ; 
    SBT* sbt ; 
#endif

    Frame* frame ; 
    SMeta* meta ; 
    double dt ; 

    scontext*    sctx ; 
    QSim*        sim ; 
    QEvent*      event ;  

    const char* desc() const ; 

private:
    static Params* InitParams( int raygenmode, const SGLM* sglm  ) ; 
    static void InitEvt(  CSGFoundry* fd  ); 
    static void InitMeta( const SSim* ssim ); 
    static void InitSim(  SSim* ssim ); 
    static void InitGeo(  CSGFoundry* fd ); 

public:
    static CSGOptiX* Create(CSGFoundry* foundry ); 

    static scontext* SCTX ; 
    static void SetSCTX(); 
    static std::string GetGPUMeta(); 

    CSGOptiX(const CSGFoundry* foundry ); 

private:
    void init(); 
    void initCtx(); 
    void initPIP(); 
    void initSBT(); 
    void initFrameBuffer();
    void initCheckSim(); 
    void initStack(); 
    void initParams();
    void initGeometry();
    void initRender();
    void initSimulate();
    void initFrame(); 
 private: 
    void setTop(const char* tspec); 
 public: 
    static const char* Top() ; 

    void setFrame(); 
    void setFrame(const char* moi);
    void setFrame(const float4& ce); 
    void setFrame(const sframe& fr_); 


    void prepareRenderParam(); 
    void prepareSimulateParam(); 
    void prepareParam(); 

 private: 
    double launch(); 
 private: 
    const char* getRenderStemDefault() const ; 
 public: 
    double render(const char* name=nullptr);   // formerly render_snap
    double simtrace(int eventID); 
    double simulate(int eventID); 
    double proceed(); 
 private: 
    // these launch  methods fulfil SCSGOptix protocal base
    // the latter two get invoked from QSim::simtrace QSim::simulate following genstep uploading   
    double render_launch();   
    double simtrace_launch(); 
    double simulate_launch();    
 public: 
    const CSGFoundry* getFoundry() const ; 
    static std::string AnnotationTime( double dt, const char* extra=nullptr ); 
    static std::string Annotation( double dt, const char* bot_line, const char* extra=nullptr ); 
    const char* getDefaultSnapPath() const ; 
    void snap(const char* path=nullptr, const char* bottom_line=nullptr, const char* top_line=nullptr, unsigned line_height=24);  // part of SRenderer protocol base


#ifdef WITH_FRAME_PHOTON
    void writeFramePhoton(const char* dir, const char* name);
#endif
    int  render_flightpath(); 

    void saveMeta(const char* jpg_path) const ;
    const NP* getIAS_Instances(unsigned ias_idx=0) const ; // for debug only 
    void save(const char* dir) const ;  

    static constexpr const char* CTX_LOGNAME = "CSGOptiX__Ctx.log"  ; 
    void write_Ctx_log(const char* dir=nullptr) const ;  

    static int   _OPTIX_VERSION() ; 
};


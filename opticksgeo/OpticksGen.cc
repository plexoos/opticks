
#include "NCSG.hpp"
#include "NPY.hpp"
#include "FabStepNPY.hpp"
#include "TorchStepNPY.hpp"
#include "NEmitPhotonsNPY.hpp"

#include "NParameters.hpp"
#include "GLMFormat.hpp"

#include "GGeoBase.hh"
#include "GBndLib.hh"

#include "OpticksPhoton.h"
#include "Opticks.hh"
#include "OpticksCfg.hh"
#include "OpticksHub.hh"
#include "OpticksEvent.hh"
#include "OpticksActionControl.hh"

#include "OpticksGen.hh"

#include "PLOG.hh"


OpticksGen::OpticksGen(OpticksHub* hub) 
   :
   m_hub(hub),
   m_ok(hub->getOpticks()),
   m_cfg(m_ok->getCfg()),
   m_ggb(hub->getGGeoBase()),
   m_blib(m_ggb->getBndLib()),
   m_lookup(hub->getLookup()),
   m_torchstep(NULL),
   m_fabstep(NULL),
   m_input_gensteps(NULL),
   m_csg_emit(hub->findEmitter()),
   m_emitter(m_csg_emit ? new NEmitPhotonsNPY(m_csg_emit, EMITSOURCE) : NULL ),
   m_input_photons(NULL),
   m_source_code( m_emitter ? EMITSOURCE : m_ok->getSourceCode() )
{
    init() ;
}

unsigned OpticksGen::getSourceCode() const 
{
    return m_source_code ;  
}


void OpticksGen::init()
{
    if(m_emitter) 
    {
        initFromEmitter();
    }
    else
    { 
        initFromGensteps();
    }
}



void OpticksGen::initFromEmitter()
{
    // emitter bits and pieces get dressed up 
    // perhaps make a class to do this ?   

    NPY<float>* iox = m_emitter->getPhotons();
    setInputPhotons(iox);

    m_fabstep = m_emitter->getFabStep();

    NPY<float>* gs = m_emitter->getFabStepData();
    assert( gs );

    gs->setAux((void*)iox); // under-radar association of input photons with the fabricated genstep

    // this gets picked up by OpticksRun::setGensteps 


    const char* oac_ = "GS_EMITSOURCE" ;  

    gs->addActionControl(OpticksActionControl::Parse(oac_));

    OpticksActionControl oac(gs->getActionControlPtr());
    setInputGensteps(gs);

    LOG(info) << "OpticksGen::initFromEmitter getting input photons and shim genstep "
              << " input_photons " << m_input_photons->getNumItems()
              << " oac : " << oac.description("oac") 
              ; 
}




void OpticksGen::initFromGensteps()
{
    if(m_ok->isNoInputGensteps() || m_ok->isEmbedded())
    {
        LOG(warning) << "OpticksGen::initFromGensteps SKIP as isNoInputGensteps OR isEmbedded  " ; 
        return ; 
    } 

    const char* type = m_ok->getSourceType();
    unsigned code = m_ok->getSourceCode();

    LOG(debug) << "OpticksGen::initFromGensteps" 
               << " code " << code
               << " type " << type
               ;

    NPY<float>* gs = makeInputGensteps(code) ; 
    assert( gs );
    setInputGensteps(gs);
}


NPY<float>* OpticksGen::makeInputGensteps(unsigned code)
{
    NPY<float>* gs = NULL ; 
    if( code == FABRICATED || code == MACHINERY  )
    {
        m_fabstep = makeFabstep();
        gs = m_fabstep->getNPY();
    }
    else if(code == TORCH)
    {
        m_torchstep = makeTorchstep() ;
        gs = m_torchstep->getNPY();
    }
    else if( code == CERENKOV || code == SCINTILLATION || code == NATURAL )
    {
        gs = loadGenstepFile("GS_LOADED,GS_LEGACY");
    }
    else if( code == G4GUN  )
    {
        if(m_ok->existsGenstepPath())
        {
             gs = loadGenstepFile("GS_LOADED");
        }
        else
        {
             std::string path = m_ok->getGenstepPath();
             LOG(warning) <<  "G4GUN running, but no gensteps at " << path 
                          << " LIVE G4 is required to provide the gensteps " 
                          ;
        }
    }
    return gs ; 
}




NPY<float>* OpticksGen::getInputGensteps() const 
{
    return m_input_gensteps ; 
}

FabStepNPY* OpticksGen::getFabStep() const 
{
    return m_fabstep ; 
}

GenstepNPY* OpticksGen::getGenstepNPY() const 
{
    unsigned source_code = getSourceCode();

    GenstepNPY* gsnpy = NULL ; 

    if(source_code == TORCH)
    {
        gsnpy = dynamic_cast<GenstepNPY*>(getTorchstep()) ;
    } 
    else if( source_code == EMITSOURCE )
    {
        gsnpy = dynamic_cast<GenstepNPY*>(getFabStep());
    }
    return gsnpy ; 
}






NPY<float>* OpticksGen::getInputPhotons() const
{
    return m_input_photons ; 
}

void OpticksGen::setInputGensteps(NPY<float>* gs)
{
    m_input_gensteps = gs ;  
    if(gs)  // will be NULL for G4GUN for example
    {
        gs->setBufferSpec(OpticksEvent::GenstepSpec(m_ok->isCompute()));
    }
}

void OpticksGen::setInputPhotons(NPY<float>* iox)
{
    m_input_photons = iox ;  
    if(iox) 
    {
        iox->setBufferSpec(OpticksEvent::SourceSpec(m_ok->isCompute()));
    }
}





TorchStepNPY* OpticksGen::getTorchstep() const // used by CGenerator for  cfg4-/CTorchSource duplication
{
    return m_torchstep ; 
}




void OpticksGen::targetGenstep( GenstepNPY* gs )
{
    // targetted positioning and directioning of the torch requires geometry info, 
    // which is not available within npy- so need to externally setFrameTransform
    // based on integer frame volume index

    if(gs->isFrameTargetted())
    {    
        LOG(info) << "OpticksGen::targetGenstep frame targetted already  " << gformat(gs->getFrameTransform()) ;  
    }    
    else 
    {   
        if(m_hub)
        {
            glm::ivec4& iframe = gs->getFrame();
            glm::mat4 transform = m_hub->getTransform( iframe.x );
            LOG(info) << "OpticksGen::targetGenstep setting frame " << iframe.x << " " << gformat(transform) ;  
            gs->setFrameTransform(transform);
        }
        else
        {
            LOG(warning) << "OpticksGen::targetGenstep SKIP AS NO GEOMETRY " ; 
        }

    }    
}


void OpticksGen::setMaterialLine( GenstepNPY* gs )
{
    if(!m_blib)
    {
        LOG(warning) << "OpticksGen::setMaterialLine no blib, skip setting material line " ;
        return ; 
    }

   // translation from a string name from config into a mat line
   // only depends on the GBndLib being loaded, so no G4 complications
   // just need to avoid trying to translate the matline later

   const char* material = gs->getMaterial() ;

   if(material == NULL)
      LOG(fatal) << "NULL material from GenstepNPY, probably missed material in torch config" ;
   assert(material);

   unsigned int matline = m_blib->getMaterialLine(material);
   gs->setMaterialLine(matline);  

   LOG(debug) << "OpticksGen::setMaterialLine"
              << " material " << material 
              << " matline " << matline
              ;
}




FabStepNPY* OpticksGen::makeFabstep()
{
    FabStepNPY* fabstep = new FabStepNPY(FABRICATED, 10, 10 );

    const char* material = m_ok->getDefaultMaterial();
    fabstep->setMaterial(material);

    targetGenstep(fabstep);  // sets frame transform
    setMaterialLine(fabstep);
    fabstep->addActionControl(OpticksActionControl::Parse("GS_FABRICATED"));
    return fabstep ; 
}

TorchStepNPY* OpticksGen::makeTorchstep()
{
    TorchStepNPY* torchstep = m_ok->makeSimpleTorchStep();
    targetGenstep(torchstep);  // sets frame transform
    setMaterialLine(torchstep);
    torchstep->addActionControl(OpticksActionControl::Parse("GS_TORCH"));

    bool torchdbg = m_ok->hasOpt("torchdbg");
    torchstep->addStep(torchdbg);  // copyies above configured step settings into the NPY and increments the step index, ready for configuring the next step 

    NPY<float>* gs = torchstep->getNPY();
    if(torchdbg) gs->save("$TMP/torchdbg.npy");

    return torchstep ; 
}

NPY<float>* OpticksGen::loadGenstepFile(const char* label)
{
    NPY<float>* gs = m_ok->loadGenstep();
    if(gs == NULL)
    {
        LOG(fatal) << "OpticksGen::loadGenstepFile FAILED" ;
        m_ok->setExit(true);
        return NULL ; 
    } 
    gs->setLookup(m_lookup);

    int modulo = m_cfg->getModulo();

    NParameters* parameters = gs->getParameters();
    parameters->add<int>("Modulo", modulo );
    if(modulo > 0) 
    {    
        parameters->add<std::string>("genstepOriginal",   gs->getDigestString()  );
        LOG(warning) << "OptickGen::loadGenstepFile applying modulo scaledown " << modulo ;
        gs = NPY<float>::make_modulo(gs, modulo);
        parameters->add<std::string>("genstepModulo",   gs->getDigestString()  );
    }    
    gs->addActionControl(OpticksActionControl::Parse(label));
    return gs ; 
}




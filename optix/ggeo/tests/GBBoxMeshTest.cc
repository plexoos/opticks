#include "GCache.hh"
#include "GMergedMesh.hh"
#include "GBBoxMesh.hh"

#include "OpticksResource.hh"
#include "NLog.hpp"

int main(int argc, char** argv)
{
    GCache gc("GGEOVIEW_");
    unsigned int ridx = 1 ;  

    OpticksResource* resource = gc.getResource();

    std::string mmpath = resource->getMergedMeshPath(ridx);
    GMergedMesh* mm = GMergedMesh::load(mmpath.c_str(), ridx);

    mm->Summary("mm loading");
    mm->dump("mm dump", 10);
    mm->dumpSolids("dumpSolids");

    unsigned int numSolids = mm->getNumSolids();

    LOG(info) << "mm numSolids " << numSolids  ;

    GBBoxMesh* bb = GBBoxMesh::create(mm);


    return 0 ;
}

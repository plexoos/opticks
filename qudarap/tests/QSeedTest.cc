#include <vector>
#include <cuda_runtime.h>
#include "scuda.h"
#include "SBuf.hh"
#include "QSeed.hh"

int main(int argc, char** argv)
{
    std::vector<int> counts = { 3, 5, 2, 0, 1, 3, 4, 2, 4 };
    std::vector<int> xseeds ; 
    QSeed::ExpectedSeeds(xseeds, counts); 

    SBuf<quad6> gs = QSeed::UploadFakeGensteps(counts) ; 
    SBuf<int> se = QSeed::CreatePhotonSeeds(gs); 
    se.download_dump("QSeed::CreatePhotonSeeds"); 

    std::vector<int> seeds ; 
    se.download(seeds); 

    int mismatch = QSeed::CompareSeeds( seeds, xseeds ); 
    std::cout << " mismatch " << mismatch << std::endl ; 
    assert( mismatch == 0 ); 

    return 0 ;  
}





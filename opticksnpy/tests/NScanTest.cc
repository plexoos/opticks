/*

NScanTest $TMP/tgltf/extras

*/

#include "BStr.hh"
#include "BFile.hh"
#include "BOpticksResource.hh"

#include "NPY.hpp"
#include "NCSG.hpp"
#include "NScan.hpp"
#include "NNode.hpp"
#include "NSceneConfig.hpp"

#include "PLOG.hh"
#include "NPY_LOG.hh"


int main(int argc, char** argv)
{
    PLOG_(argc, argv);
    NPY_LOG__ ;  


    BOpticksResource okr ;  // no Opticks at this level 
    std::string treedir = okr.getDebuggingTreedir(argc, argv);  // uses debugging only IDPATH envvar
    const char* basedir = treedir.c_str(); 

    const char* gltfconfig = "csg_bbox_parsurf=1" ;

    int verbosity = 0 ; 
    std::vector<NCSG*> trees ;

    if(BFile::pathEndsWithInt(basedir))
    {
        NCSG* csg = NCSG::LoadCSG(basedir, gltfconfig);
        if(csg) trees.push_back(csg);   
    }
    else
    {
        NCSG::DeserializeTrees( basedir, trees, verbosity );
    }

    int ntree = trees.size() ; 
    verbosity = ntree == 1 ? 4 : 0 ; 

    LOG(info) 
          << " NScanTest autoscan trees " 
          << " basedir " << basedir  
          << " ntree " << ntree 
          << " verbosity " << verbosity 
          ; 



    const unsigned MESSAGE_NZERO = 1000001 ;  

    typedef std::map<unsigned, unsigned> MUU ; 
    MUU counts ; 

    //float mmstep = 1.f ; 
    float mmstep = 0.1f ; 

    std::vector<NScan*> scans ; 

    for(unsigned i=0 ; i < trees.size() ; i++)
    {
        NCSG* csg = trees[i]; 
        nnode* root = csg->getRoot();

        NScan* scan = new NScan(*root, verbosity);
        unsigned nzero = scan->autoscan(mmstep);
        const std::string& msg = scan->get_message();
        if(!msg.empty()) counts[MESSAGE_NZERO]++ ; 

        scans.push_back(scan);

        counts[nzero]++ ; 
    }


    unsigned total = trees.size() ;
    LOG(info) << " autoscan non-zero counts"
              << " trees " << total 
              << " mmstep "<< mmstep 
               ; 
    for(MUU::const_iterator it=counts.begin() ; it != counts.end() ; it++)
    {
        std::cout 
           << " nzero " << std::setw(4) << it->first   
           << " count " << std::setw(4) << it->second
           << " frac " << float(it->second)/float(total) 
           << std::endl 
           ;
    }
    
    for(MUU::const_iterator it=counts.begin() ; it != counts.end() ; it++)
    {
        unsigned nzero = it->first  ;
        bool expect = nzero == 2 || nzero == 4 ; 
        bool dump = !expect  ; 

        std::cout 
           << std::endl 
           << " nzero " << std::setw(4) << it->first   
           << " count " << std::setw(4) << it->second
           << " frac " << float(it->second)/float(total) 
           << std::endl 
           ;

        if(!dump) continue ; 

        

        for(unsigned i=0 ; i < scans.size() ; i++)
        {
            NScan* scan = scans[i] ; 
            NCSG*  csg = trees[i] ; 
            nnode* root = csg->getRoot();

            bool with_nzero = scan->get_nzero() == nzero ;
            bool with_message = scan->has_message() && nzero == MESSAGE_NZERO ; 
            unsigned nprim = root->get_num_prim() ; 



            if(with_nzero || with_message)
            {
                std::cout 
                     << " i " << std::setw(4) << i 
                     << " nzero " << std::setw(4) << nzero 
                     << " NScanTest " << std::setw(4) << csg->getTreeNameIdx()
                     //<< std::left << std::setw(40) << csg->getTreeDir()  << std::right
                     //<< " treeNameIdx " << csg->getTreeNameIdx()
                     << " soname " << std::setw(40) << csg->soname()  
                     << " tag " << std::setw(10) << root->tag()
                     << " nprim " << std::setw(4) << nprim
                     << " typ " << std::setw(20) << root->get_type_mask_string()
                     << " msg " << scan->get_message()
                     << std::endl 
                     ;
            }
        }
    }
    return 0 ; 
}








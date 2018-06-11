#pragma once

#include <string>
#include <vector>
#include <map>

#include "NGLM.hpp"
#include "OpticksCSG.h"

template <typename T> class NPY ; 
#include "NPY_API_EXPORT.hh"

/**
NCSG
======

cf dev/csg/csg.py 

* NB two constructors:

  1. deserialize from a python written treedir
  2. serialize an nnode tree

* hmm the raw loaded buffer lacks bounding boxes when derived from user input, 
  to get those need to vivify the CSG tree and the export it back to the buffer


TODO SOMETIME : break up the monolith
--------------------------------------

* Does too much for one class. 
* A pivotal class, so rejigging it will be time consuming. 

* perhaps transport buffers could live in a separate class fairly easily  


Where NCSG is used
-------------------

void GGeoTest::loadCSG(const char* csgpath, std::vector<GSolid*>& solids)

    GGeoTest is the primary user of NCSG, with method GGeoTest::loadCSG
    invoking NCSG::Deserialize to create vectors of csg trees. 
    Each tree is converted into a GSolid by GMaker::makeFromCSG(NCSG* tree).  

    This python defined CSG node tree approach has replaced
    the old bash configured GGeoTest::createCsgInBox.


GSolid* GMaker::makeFromCSG(NCSG* csg)

    Coordinates:

    * NPolygonizer -> GMesh 
    * GParts::make, create analytic description
    * GSolid, container for GMesh and GParts

GParts* GParts::make( NCSG* tree)

    Little to do, just bndlib hookup :
    huh but thats done in GMaker::makeFromCSG too...

    * Is GParts is serving any purpose for NCSG ?
      YES : it provides merging of analytic solids
      in parallel with the merging of triangulated meshes


**/

struct nvec4 ; 
union nquad ; 
struct nnode ; 
struct nmat4pair ; 
struct nmat4triple ; 
struct nbbox ; 
struct NSceneConfig ; 
struct NNodeNudger ; 


class NParameters ; 
class NNodePoints ; 
class NNodeUncoincide ; 
class NTrianglesNPY ;

class NPY_API NCSG {
        friend class  NCSGList ; 
        friend struct NCSGLoadTest ; 
        typedef std::map<std::string, nnode*> MSN ; 
    public:
        enum { NJ = 4, NK = 4, MAX_HEIGHT = 10 };

        static const char* FILENAME ; 
        static const char* TREE_META ; 
        static const char* NODE_META ; 

        static const char* PLANES ; 
        static const char* SRC_FACES ; 
        static const char* SRC_VERTS ;
 
        static const unsigned NTRAN ; 
        static const float SURFACE_EPSILON ; 

        static std::string TestVolumeName(const char* shapename, const char* suffix, int idx) ; 
        std::string getTestLVName() const ;
        std::string getTestPVName() const ;

        static NCSG* FromNode(nnode* root, const NSceneConfig* config);
        static NCSG* LoadCSG(const char* treedir, const char* gltfconfig);
        static NCSG* LoadTree(const char* treedir, const NSceneConfig* config );

        static unsigned NumNodes(unsigned height);

        static std::string TxtPath(const char* treedir);
        static bool Exists(const char* treedir);   // compat : pass thru to ExistsDir
        static bool ExistsDir(const char* treedir);   // just checks existance of dir
        static bool ExistsTxt(const char* treedir);  // looks for FILENAME (csg.txt) in the treedir

        static std::string MetaPath(const char* treedir, int idx=-1);
        static bool        ExistsMeta(const char* treedir, int idx=-1);
        static NParameters* LoadMetadata(const char* treedir, int idx=-1);

        NNodeUncoincide* make_uncoincide() const ;
        NNodeNudger*     make_nudger() const ;
        unsigned         get_num_coincidence() const ;
        std::string      desc_coincidence() const ;

        void adjustToFit( const nbbox& container, float scale, float delta ) const ;
    public:
        NTrianglesNPY* polygonize();
        NTrianglesNPY* getTris();


    public:
        // passthru to root
        unsigned    get_type_mask() const ;
        unsigned    get_oper_mask() const ;
        unsigned    get_prim_mask() const ;
        std::string get_type_mask_string() const ;
    public:
        nbbox bbox_analytic() const ;
        nbbox bbox_surface_points() const ;

        const std::vector<glm::vec3>& getSurfacePoints() const ;
        unsigned getNumSurfacePoints() const ;
        float    getSurfaceEpsilon() const ; 
        static   glm::uvec4 collect_surface_points(std::vector<glm::vec3>& surface_points, const nnode* root, const NSceneConfig* config, unsigned verbosity, float epsilon );
    private:
        glm::uvec4 collect_surface_points();
    public:
        template<typename T> void setMeta(const char* key, T value);
    public:
        template<typename T> T getMeta(const char* key, const char* fallback ) const ;
        std::string lvname() const ;
        std::string soname() const ;
        int treeindex() const ;
        int depth() const ;
        int nchild() const ;
        bool isSkip() const ;
        bool is_uncoincide() const ;
        std::string meta() const ;
        std::string smry();
    public:
        // used by --testauto 
        void setEmit(int emit);  
        void setEmitConfig(const char* emitconfig);  
    public:
        bool        isEmit() const ;
        int         getEmit() const ;
        const char* getEmitConfig() const ;
    public:
        void dump(const char* msg="NCSG::dump");
        void dump_surface_points(const char* msg="NCSG::dump_surface_points", unsigned dmax=20) const ;

        std::string desc();
        std::string brief() const ;
   public:
        void setIsUsedGlobally(bool usedglobally);
   public:
        const char*  getBoundary() const ;
        const char*  getTreeDir() const ;
        const char*  getTreeName() const ;
        int          getTreeNameIdx() const ;  // will usually be the lvidx

        unsigned     getIndex() const ;
        int          getVerbosity() const ;
    public:
        bool         isContainer() const ;
        float        getContainerScale() const ;
        bool         isUsedGlobally() const ;

        NPY<float>*  getNodeBuffer();
        NPY<float>*  getTransformBuffer();
        NPY<float>*  getGTransformBuffer();
        NPY<float>*  getPlaneBuffer();
        NParameters* getMetaParameters();
        NParameters* getNodeMetadata(unsigned idx) const ;


        unsigned     getNumNodes() const ;
        unsigned     getNumTransforms();

        unsigned     getHeight() const ;
        nnode*       getRoot() const ;
        OpticksCSG_t getRootType() const ;  
        unsigned getNumTriangles();
    public:
        void check();
        void check_r(nnode* node); 
        void setIndex(unsigned index);
        void setVerbosity(int verbosity);
    private:
        // Deserialize
        NCSG(const char* treedir);
         // Serialize 
        NCSG(nnode* root);

    public:
        // for --testauto
        void setBoundary(const char* boundary);
    private:
        // Deserialize branch 
        void setConfig(const NSceneConfig* config);
        unsigned getTypeCode(unsigned idx);
        unsigned getTransformIndex(unsigned idx);
        bool     isComplement(unsigned idx);
        nquad getQuad(unsigned idx, unsigned j);
    private:
        void load();
        void loadMetadata();
        void increaseVerbosity(int verbosity);
        void loadNodes();
        void loadNodeMetadata();
        void loadTransforms();
        void loadPlanes();
        void loadSrcVerts();
        void loadSrcFaces();
    private:
        void import();
        void postimport();
        void postimport_uncoincide();
        void postimport_autoscan();

        nnode* import_r(unsigned idx, nnode* parent=NULL);
        nnode* import_primitive( unsigned idx, OpticksCSG_t typecode );
        nnode* import_operator( unsigned idx, OpticksCSG_t typecode );
        void import_planes(nnode* node);
        void import_srcvertsfaces(nnode* node);
    private:
        nmat4pair*   import_transform_pair(unsigned itra);
        nmat4triple* import_transform_triple(unsigned itra);
        unsigned addUniqueTransform( const nmat4triple* gtransform );
    private:
         // Serialize branch
        void export_r(nnode* node, unsigned idx);
        void export_();
    private:
        NParameters* m_meta ; 
        const char* m_treedir ; 
        unsigned     m_index ; 
        float        m_surface_epsilon ; 
        int          m_verbosity ;  
        bool         m_usedglobally ; 

        nnode*           m_root ;  
        NNodePoints*     m_points ; 
        NNodeUncoincide* m_uncoincide ; 
        NNodeNudger*     m_nudger ; 

        NPY<float>* m_nodes ; 
        NPY<float>* m_transforms ; 
        NPY<float>* m_gtransforms ; 
        NPY<float>* m_planes ;
        NPY<float>* m_srcverts ;
        NPY<int>*   m_srcfaces ;

        std::map<unsigned, NParameters*> m_nodemeta ; 

        unsigned    m_num_nodes ; 
        unsigned    m_num_transforms ; 
        unsigned    m_num_planes ;

        unsigned    m_num_srcverts ;
        unsigned    m_num_srcfaces ;
 
        unsigned    m_height ; 
        const char*         m_boundary ; 
        const NSceneConfig* m_config ; 

        glm::vec3   m_gpuoffset ; 
        int         m_container ;  
        float       m_containerscale ;  

        
        NTrianglesNPY*         m_tris ; 
        std::vector<glm::vec3> m_surface_points ; 

};



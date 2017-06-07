#pragma once

#include <vector>
struct nuv ; 

#include "NOpenMeshType.hpp"

template <typename T>
struct NPY_API  NOpenMeshProp
{
    typedef typename T::VertexHandle VH ; 
    typedef typename T::FaceHandle   FH ; 

    enum
    {
        ALL_OUTSIDE_OTHER = 0,
        ALL_INSIDE_OTHER = 7  
    };


    static const char* F_INSIDE_OTHER ; 
    static const char* F_GENERATION ; 
    static const char* F_IDENTITY ;
 
#ifdef WITH_V_GENERATION
    static const char* V_GENERATION ; 
#endif
    static const char* V_SDF_OTHER ; 
    static const char* V_PARAMETRIC ; 
    static const char* H_BOUNDARY_LOOP ; 

    NOpenMeshProp( T& mesh );
    void init();
    void init_status();
    void update_normals();

    bool is_facemask_face( FH fh, int facemask=-1 ) const ;
    int  get_facemask( FH fh ) const ;
    void set_facemask( FH fh, int facemask );

    bool is_identity_face( FH fh, const std::vector<int>& identity ) const ;
    bool is_identity_face( FH fh, int identity ) const ;
    int  get_identity( FH fh ) const ;
    void set_identity( FH fh, int identity );


    int  get_fgeneration( FH fh ) const ;
    void set_fgeneration( FH fh, int fgen );
    void increment_fgeneration( FH fh );
    bool is_even_fgeneration(const FH fh, int mingen) const ;
    bool is_odd_fgeneration(const FH fh, int mingen) const ;
    void set_fgeneration_all( int fgen );

#ifdef WITH_V_GENERATION
    int  get_vgeneration( VH vh ) const ;
    void set_vgeneration( VH vh, int vgen );
    void increment_vgeneration( VH vh );
    bool is_even_vgeneration(const VH vh, int mingen) const ;
    bool is_odd_vgeneration(const VH vh, int mingen) const ;
    void set_vgeneration_all( int vgen );
#endif

    nuv  get_uv( VH vh ) const ; 
    void set_uv( VH vh, nuv uv )  ; 

    T& mesh  ;

    OpenMesh::VPropHandleT<nuv>    v_parametric ;
    OpenMesh::VPropHandleT<float>  v_sdf_other ;
#ifdef WITH_V_GENERATION
    OpenMesh::VPropHandleT<int>    v_generation ;
#endif

    OpenMesh::FPropHandleT<int>    f_inside_other ;
    OpenMesh::FPropHandleT<int>    f_generation ;
    OpenMesh::FPropHandleT<int>    f_identity ;

    OpenMesh::HPropHandleT<int>    h_boundary_loop ;

};
 




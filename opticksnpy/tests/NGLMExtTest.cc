

#include "NGLMExt.hpp"
#include "GLMFormat.hpp"

#include <glm/gtc/epsilon.hpp>


#include "PLOG.hh"

void test_stream()
{
    glm::ivec3 iv[4] = {
       {1,2,3},
       {10,20,30},
       {100,200,300},
       {1000,2000,3},
    };

    for(int i=0 ; i < 4 ; i++)
        std::cout << std::setw(20) << glm::to_string(iv[i]) << std::endl ; 

    for(int i=0 ; i < 4 ; i++)
        std::cout << std::setw(20) << iv[i] << std::endl ; 



    glm::vec3 fv[4] = {
       {1.23,2.45,3},
       {10.12345,20,30.2235263},
       {100,200,300},
       {1000,2000,3},
    };

    for(int i=0 ; i < 4 ; i++)
        std::cout << std::setw(20) << glm::to_string(fv[i]) << std::endl ; 

    for(int i=0 ; i < 4 ; i++)
        std::cout << std::setw(20) << fv[i] << std::endl ; 

}


void test_invert_tr()
{
    LOG(info) << "test_invert_tr" ;
 
    glm::vec3 axis(1,1,1);
    glm::vec3 tlat(0,0,100) ; 
    float angle = 45.f ; 

    glm::mat4 tr(1.f) ;
    tr = glm::translate(tr, tlat );
    tr = glm::rotate(tr, angle, axis );

    glm::mat4 irit = nglmext::invert_tr( tr );

    //std::cout << gpresent(" tr ", tr) << std::endl ; 
    //std::cout << gpresent(" irit ", irit ) << std::endl ; 

    nmat4pair mp(tr, irit);
    std::cout << " mp " << mp << " dig " << mp.digest() << std::endl ; 

}


void test_nmat4triple_make_translated()
{
    LOG(info) << "test_nmat4triple_make_translated" ;

    glm::vec3 tlat(0,0,100) ; 
    glm::vec4 trot(0,0,1,0) ; 
    glm::vec3 tsca(1,1,1) ; 

    glm::mat4 trs = nglmext::make_transform("trs", tlat, trot, tsca );
    nmat4triple tt0(trs);

    glm::vec3 tlat2(-100,0,0) ; 
    bool reversed(true);
    nmat4triple* tt1 = tt0.make_translated( tlat2, reversed );

    std::cout 
         << " tt0 " << tt0 
         << " tt1 " << *tt1
         << std::endl 
        ;  
}



int main(int argc, char** argv)
{
    PLOG_(argc, argv) ; 

    test_stream();
    test_invert_tr();
    test_nmat4triple_make_translated();

    return 0 ; 
}

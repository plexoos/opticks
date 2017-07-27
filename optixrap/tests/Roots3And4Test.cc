#include "OptiXTest.hh"

#include "NPY.hpp"

#include "OXRAP_LOG.hh"
#include "PLOG.hh"


int main( int argc, char** argv ) 
{
    PLOG_(argc, argv);
    OXRAP_LOG__ ; 

    optix::Context context = optix::Context::create();

    OptiXTest* test = new OptiXTest(context, "Roots3And4Test.cu", "Roots3And4Test") ;
    test->Summary(argv[0]);

    //unsigned width = 512 ; 
    //unsigned height = 512 ; 

    unsigned width = 1 ; 
    unsigned height = 1 ; 




    // optix::Buffer buffer = context->createBuffer( RT_BUFFER_OUTPUT, RT_FORMAT_FLOAT4, width, height );
    optix::Buffer buffer = context->createBuffer( RT_BUFFER_OUTPUT, RT_FORMAT_FLOAT4, width*height );
    context["output_buffer"]->set(buffer);

    context->validate();
    context->compile();
    context->launch(0, width, height);


    NPY<float>* npy = NPY<float>::make(width, height, 4) ;
    npy->zero();

    void* ptr = buffer->map() ; 
    npy->read( ptr );
    buffer->unmap(); 

    const char* path = "$TMP/Roots3And4Test.npy";
    std::cerr << "save result npy to " << path << std::endl ; 
 
    npy->save(path);


    return 0;
}

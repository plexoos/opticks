#include "GCache.hh"
#include "GMaterialLib.hh"

int main()
{
    GCache gc("GGEOVIEW_");

    GMaterialLib* lib = GMaterialLib::load(&gc);

    lib->dump();

    return 0 ;
}


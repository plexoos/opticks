#include "app.hh"

int main(int argc, char** argv)
{
    App app("GGEOVIEW_", argc, argv); 
    if(app.isExit()) exit(EXIT_SUCCESS);

    app.prepareScene();

    app.loadGeometry();
    if(app.isExit()) exit(EXIT_SUCCESS);

    app.uploadGeometry();

    bool nooptix = app.hasOpt("nooptix");
    bool noevent = app.hasOpt("noevent");
    bool save    = app.hasOpt("save");

    if(!nooptix)
    {

        app.loadGenstep();

        app.uploadEvt();    // allocates GPU buffers with OpenGL glBufferData

        app.seedPhotonsFromGensteps();

        app.initRecords();


        app.configureOptiXGeometry();

        app.prepareOptiX();


        if(!noevent)
        {
            app.preparePropagator();

            app.propagate();

            app.indexEvt();

            if(save)
            {

                app.downloadEvt();

                app.indexEvtOld();  // indexing that depends on downloading to host 
            }
        }

        app.makeReport();
    }

    app.prepareGUI();

    app.renderLoop();

    app.cleanup();

    exit(EXIT_SUCCESS);
}


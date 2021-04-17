// ./STTFTest.sh 

#include "STTF.hh"

#define SIMG_IMPLEMENTATION 1 
#include "SIMG.hh"


const char* TEXT = "STTFTest : the quick brown fox jumps over the lazy dog 0.123456789 "  ; 

int main(int argc, char** argv)
{
    const char* path = argc > 1 ? argv[1] : "/tmp/STTFTest.jpg" ; 
    const char* text = argc > 2 ? argv[2] : TEXT ; 

    STTF sttf ; 
    
    int width = 1280;
    int height = 720; 
    int channels = 4 ; 
    int line_height = 32 ; 
    int offset = 0 ; 

    int magenta[4] = {255,0,255,0} ; 
    int black[4] = {0,0,0,0} ; 

    unsigned char* data = (unsigned char*)calloc(width * height * channels, sizeof(unsigned char));
    sttf.render_background( data,        channels, width, height,      magenta ) ;

    sttf.render_background( data+offset, channels, width, line_height, black ) ;
    sttf.render_text(       data+offset, channels, width, line_height, text ) ;

    offset = width*height*channels/2 ;   
    sttf.render_background( data+offset, channels, width, line_height, black ) ;
    sttf.render_text(       data+offset, channels, width, line_height, text ) ;

    offset = width*(height-line_height-1)*channels ;    // -1 to avoid stepping off the end and segmenting 
    sttf.render_background( data+offset, channels, width, line_height, black ) ;
    sttf.render_text(       data+offset, channels, width, line_height, text ) ;


    SIMG img(width, height, channels, data ); 

    int quality = 50 ; 
    printf("STTFTest : writing to %s quality %d \n", path, quality ); 
    img.writeJPG(path, quality ); 
 
    free(data);

    return 0 ; 
}

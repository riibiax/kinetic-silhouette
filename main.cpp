#include "ofMain.h"  
#include "testApp.h"  
#include "ofAppGlutWindow.h"  

int main( ){  
    ofAppGlutWindow window;  
    //  ofSetupOpenGL(&window, 1024,768, OF_WINDOW);         // <-------- setup the GL context  
    
    ofSetupOpenGL(&window, 1280,800, OF_FULLSCREEN);   

    ofRunApp(new testApp());  
    
} 
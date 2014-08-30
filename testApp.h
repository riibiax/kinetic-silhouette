#pragma once  

#include "ofMain.h"  
#include "ofxOpenCv.h"  
#include "ofxKinect.h"


#define BUF 16

class testApp : public ofBaseApp{  
    
    public:  
        void    setup();  
        void    update();  
        void    draw();  
        void    keyPressed(int key);
        void    exit();
  
        void    setupKinect();
        void    drawPointCloud();
        bool    updateImage(ofImage img);
        void    drawGhost (int iterator, int counter);
    
        int     imageNumber;
    
        struct strucCams {
            ofPoint     pos;
            string      url;
            bool        loadImageOnce;
            ofImage     cam;
            vector< vector<ofVec3f> > vertex;
            int         counter;
            int         randomFrame;
            int         drawingTime;
        } cams[BUF];
    
        struct strucFrames {
            bool        free;
        } frames[BUF];
    
        float           timeInt;  
        float           lastTime;
    
    
        int             angle;
        int             w;
        int             h;
    	int             step;
        ofxKinect       kinect;  
        ofEasyCam       easyCam;
    
        bool            inside, outsideOnce;

        float           pointCloudMinZ , pointCloudMaxZ;
    
    
        ofxCvHaarFinder finderFace, finderBody, finderProfil;
    
        ofFbo           main, sub;
        
        ofSoundPlayer   cameraSound;
    
};
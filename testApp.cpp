#include "testApp.h"  

//verify screen size
//improve the steps
//verify images from camera
//set the min and max clouds


//--------------------------------------------------------------  
void testApp::setup(){  
    // turn off vertical sync so we can evaluate performance
   // ofSetLogLevel(OF_LOG_VERBOSE); 
    
    ofSetVerticalSync(true); 
    
    ofEnableAlphaBlending(); 
    
    lastTime = 0;  
    timeInt = 30; // seconds
    
    main.allocate(ofGetWidth()/4, ofGetHeight()/4);
    sub.allocate(ofGetWidth()/4, ofGetHeight()/4);
    
    inside = false;
    outsideOnce = true;
    
    for (int i=0; i<BUF; i++){
        frames[i].free = true;
    }
    
    imageNumber = 16;
    
    cams[0].url= "http://webcam-rade.ville-ge.ch/axis-cgi/jpg/image.cgi";
    cams[1].url = "http://81.63.187.124/oneshotimage1?136905104456"; //geneve
    cams[2].url = "http://www.lsgp.ch/webcam/webcam002.jpg"; //Prangins
    cams[3].url = "http://morschach-stoos.cam.stpartner.ch/MorschachShpPool.jpg"; //MorschachShpPool
    cams[4].url = "http://www.topcamp.ch/~transfer/MOBOTIX/topcamp.jpg"; //Full screen: Top Camp AG
    cams[5].url = "http://www.sncn.ch/public_html/Webcam.jpg"; //Thermalbad Brigerbad
    cams[6].url = "http://t-rex.ch/cam/t-rex.jpg"; //t-rex
    cams[7].url = "http://www.sky-cam.ch/parco/cam2/current1.jpg"; //adventurepark 
    cams[8].url = "http://www2.feuerwehr-baar.ch/webcams/webcam2.jpg"; //Baar - Schwimmbad LŠttich
    cams[9].url = "http://www.air-zermatt.ch/webcam/Heliport.jpg"; //zermatt helicopter
    cams[10].url = "http://www.canottieri-audax.ch/webcam/mxcam/current.jpg"; // Schaffhausen - Vordergasse
    cams[11].url = "http://www.mrtm.ch/images/live000M.jpg"; // Moto
    cams[12].url = "http://212.41.212.146:83/axis-cgi/jpg/image.cgi?resolution=640x480"; //Ausstellung Stalder-KŸchen
    cams[13].url = "http://164.14.4.250/jpg/image.jpg"; //Golfpark Moossee
    cams[14].url = "http://axis_648127.axiscam.net/jpg/image.jpg"; //Oberhofen am Thunersee
    cams[15].url = "http://www.tcstallikon.ch/webcam/cam1/archive/image.jpg"; //Tennisclub Stallikon
    
    cams[0].pos.set(0,0); 
    cams[1].pos.set(ofGetWidth()/4, 0);
    cams[2].pos.set(ofGetWidth()/2, 0); 
    cams[3].pos.set(ofGetWidth()*3/4, 0); 
    cams[4].pos.set(0, ofGetHeight()/4); 
    cams[5].pos.set(ofGetWidth()/4, ofGetHeight()/4); 
    cams[6].pos.set(ofGetWidth()/2, ofGetHeight()/4); 
    cams[7].pos.set(ofGetWidth()*3/4, ofGetHeight()/4); 
    cams[8].pos.set(0, ofGetHeight()/2); 
    cams[9].pos.set(ofGetWidth()/4, ofGetHeight()/2); 
    cams[10].pos.set(ofGetWidth()/2, ofGetHeight()/2); 
    cams[11].pos.set(ofGetWidth()*3/4, ofGetHeight()/2); 
    cams[12].pos.set(0, ofGetHeight()*3/4); 
    cams[13].pos.set(ofGetWidth()/4,ofGetHeight()*3/4); 
    cams[14].pos.set(ofGetWidth()/2,ofGetHeight()*3/4); 
    cams[15].pos.set(ofGetWidth()*3/4,ofGetHeight()*3/4); 
    
    for (int i=0; i<BUF; i++){
        cams[i].cam.loadImage(cams[i].url);
        cams[i].loadImageOnce = true;
        cams[i].vertex.clear();
        cams[i].counter= 0;
        cams[i].drawingTime= 0;
    }
     
    
    finderFace.setup("haarcascade_frontalface_default.xml");
    finderBody.setup("haarcascade_fullbody.xml");
    finderProfil.setup("haarcascade_profileface.xml");
    
    
    cameraSound.loadSound("camera.wav");
    cameraSound.setVolume(0.15f);
    cameraSound.setLoop(true);
    cameraSound.play();


	ofSetFrameRate(30);
    setupKinect();

}  

//--------------------------------------------------------------  
void testApp::setupKinect(){
	// enable depth->video image calibration
	kinect.setRegistration(true);
    
	kinect.init(false, false);
	//kinect.init(true); // shows infrared instead of RGB video image
	//kinect.init(false, false); // disable video image (faster fps)
	
	kinect.open();		// opens first available kinect
	//kinect.open(1);	// open a kinect by id, starting with 0 (sorted by serial # lexicographically))
    
    pointCloudMinZ = 200;
    pointCloudMaxZ = 2000;

    angle = 30.0f;
    kinect.setCameraTiltAngle(angle);
    
    w = 640;
	h = 400;
    step = 20; //10
}

//--------------------------------------------------------------  
void testApp::update(){

    ofSoundUpdate();

   	kinect.update();

    float currentTime= ofGetElapsedTimef();  
    
    for (int i=0; i<BUF; i++){
        if (currentTime - lastTime >= timeInt*(i+1) and cams[i].loadImageOnce){
            cams[i].cam.loadImage(cams[i].url);
            //cams[i].cam.saveImage("cam" + ofToString(i+1) + "-1.jpg");
            cams[i].loadImageOnce = false;
            if(updateImage(cams[i].cam))
                cams[i].cam.saveImage("cam" + ofToString(i+1) + ".jpg");
            if(i==15){
                lastTime = currentTime;
                for(int j=0; j<BUF; j++)
                    cams[j].loadImageOnce= true;
            }
        }
    }

    if (inside){
        if (!outsideOnce){
            outsideOnce = true;
        }
        cams[imageNumber].drawingTime = ofGetElapsedTimef();

    }
    else{
        if (outsideOnce){
            if (imageNumber<15)
                imageNumber +=1;
            else
                imageNumber=0;
            cams[imageNumber].vertex.clear();
            cams[imageNumber].counter = 0;
            while(true){
                cams[imageNumber].randomFrame = (int)ofRandom(15);
                if(frames[cams[imageNumber].randomFrame].free){
                    frames[cams[imageNumber].randomFrame].free =false;
                    break;
                }
            }
        }
        outsideOnce=false;
    }
} 

//--------------------------------------------------------------  
bool testApp::updateImage(ofImage img){  
    finderFace.findHaarObjects(img);
    finderBody.findHaarObjects(img);
    finderProfil.findHaarObjects(img);
    
    if(finderFace.blobs.size()==0 and finderBody.blobs.size()==0 and finderProfil.blobs.size()==0){
        finderFace.blobs.clear();
        finderBody.blobs.clear();
        finderProfil.blobs.clear();
        return true;
    }
    finderFace.blobs.clear();
    finderBody.blobs.clear();
    finderProfil.blobs.clear();
    return false;
}

//--------------------------------------------------------------  
void testApp::draw(){  
    ofBackground(0, 0, 0);
    ofSetColor(255, 255, 255,255);

    for (int i=0; i<BUF; i++){
        cams[i].cam.draw(cams[i].pos.x, cams[i].pos.y, ofGetWidth()/4-10, ofGetHeight()/4-10);
    }
    
    for (int i=0; i<BUF; i++){
        if (ofGetElapsedTimef() - cams[i].drawingTime>5*(i+1) and cams[i].randomFrame!=imageNumber and cams[i].vertex.size()> 0 and cams[i].counter<cams[i].vertex.size()){
            sub.begin(false);  
                ofClear(0, 0, 0); 
                easyCam.begin();
                    glEnable(GL_DEPTH_TEST);
                    ofScale(-1, 1, -1);
                    ofTranslate(0, 0, -400); // center the points a bit
                    drawGhost(i, cams[i].counter);
                    glDisable(GL_DEPTH_TEST);
                easyCam.end();
                cams[i].counter ++;
            sub.end();
            sub.draw(cams[cams[i].randomFrame].pos.x, cams[cams[i].randomFrame].pos.y, ofGetWidth()/4, ofGetHeight()/4);
        }
        else if(cams[i].vertex.size()> 0 and cams[i].counter>=cams[i].vertex.size()){
            frames[i].free =true; //not sure
        }
    }

   // kinect.draw(0,0);

    main.begin(false);  
        ofClear(0, 0, 0);
        easyCam.begin();
            glEnable(GL_DEPTH_TEST);
            ofScale(-1, 1, -1);
            ofTranslate(0, 0, -400); // 350
            drawPointCloud();
            glDisable(GL_DEPTH_TEST);
        easyCam.end();
    main.end();

    main.draw(cams[imageNumber].pos.x, cams[imageNumber].pos.y, ofGetWidth()/4, ofGetHeight()/4);
    
    //ofDrawBitmapString(ofToString(ofGetFrameRate()), 20, 20); 
} 

//--------------------------------------------------------------
void testApp::drawGhost(int iterator, int counter) {
    
    int pCount = 0;
    ofPushMatrix();
    
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
            ofVec3f vertex = cams[iterator].vertex[counter][pCount];
            ofImage croppedImage;
            croppedImage.allocate(step, step, OF_IMAGE_COLOR);
            croppedImage.cropFrom(cams[cams[iterator].randomFrame].cam, x,y, step, step);
                    
            croppedImage.getTextureReference().bind();
                    
            ofPushMatrix();

            //ofSetColor(0, 0, 0,255);
                    
            ofTranslate(vertex); 
            ofBox(ofVec3f( ), step );
            ofPopMatrix();
            croppedImage.getTextureReference().unbind();
            vertex.~ofVec3f();
            
            pCount ++;
        }
	}
    ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::drawPointCloud(){
    inside=false;   
    
    ofPushMatrix();
    vector<ofVec3f> row; // Create an empty row
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			if(kinect.getDistanceAt(x, y) > 0) {
                ofVec3f vertex = kinect.getWorldCoordinateAt(x, y);
                if ( vertex.z > pointCloudMinZ && vertex.z < pointCloudMaxZ ){
                    
                    row.push_back(vertex);
                    
                    inside = true;
                    
                    ofImage croppedImage;
                    croppedImage.allocate(step, step, OF_IMAGE_COLOR);
                    croppedImage.cropFrom(cams[imageNumber].cam, x,y, step, step);
                    
                    croppedImage.getTextureReference().bind();

                    ofPushMatrix();
                    //ofSetColor(0, 0, 0,255);
                    ofTranslate(vertex); 
                    ofBox(ofVec3f( ), step );
                    ofPopMatrix();
                    croppedImage.getTextureReference().unbind();
                }
				vertex.~ofVec3f();
			}
		}
	}
    ofPopMatrix();
    
    if (inside){
        cams[imageNumber].vertex.push_back(row);
    }
    row.~vector();

}

//--------------------------------------------------------------
void testApp::keyPressed (int key) {

    switch (key) {
                        
		case ' ':
            ofSaveScreen( ofToDataPath( ofToString( ofGetUnixTime() )  + ".jpg" ) );
            break; 
	}

}

//--------------------------------------------------------------
void testApp::exit() {
	kinect.close();
}
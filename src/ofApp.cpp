#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // kinect init and open
    kinect.setRegistration(true);
    kinect.init();
    kinect.open();
    
    ofBackground(0);
    ofSetFrameRate(60);
    
    
    // set variables
    bufferSize = 50;
    
    
    imgW = kinect.getWidth();
    imgH = kinect.getHeight();
    
    
    //set gui
    gui.setup();
    gui.add(kinectTiltAngle.set("Kinect Tilt Angle", 0, 0, 30));
    

}

//--------------------------------------------------------------
void ofApp::update(){
    // update kinect
    kinect.update();
    
    if (kinect.isFrameNew()){
        frames.push_front(kinect.getPixels());
        depthFrames.push_front(kinect.getDepthPixels());
        
        if(frames.size() > bufferSize){
            frames.pop_back();
        }
        
        if(depthFrames.size() > bufferSize){
            depthFrames.pop_back();
        }
    }
    

    
    // transfer kinect color info to ofPixel
    if (!frames.empty() && !depthFrames.empty()) {
        
        
        // set default img if ImgPixels are not allocated
        if ( !colorImgPixels.isAllocated() ) {
            colorImgPixels = frames[0];
        }
        
        if ( !depthImgPixels.isAllocated() ) {
            depthImgPixels = frames[0];
        }
        
        if ( !scanImgPixels.isAllocated() ) {
            scanImgPixels = frames[0];
        }
        
        
        // getting color and apply
        for (int y = 0; y< imgH; y++) {
            for (int x = 0; x < imgW; x++) {
                ofColor color = getPixelColor(x, y);
                colorImgPixels.setColor(x, y, color);
            }
        }
        
        // getting depth and apply
        for (int y = 0; y< imgH; y++) {
            for (int x = 0; x < imgW; x++) {
                ofColor color = getPixelDepth(x, y);
                depthImgPixels.setColor(x, y, color);
            }
        }
        
        
        // getting depth slitScan and apply
        for (int y = 0; y< imgH; y++) {
            for (int x = 0; x < imgW; x++) {
                ofColor color = getPixelSlitDepthColor(x, y);
                scanImgPixels.setColor(x, y, color);
            }
        }
        
        colorImg.setFromPixels(colorImgPixels);
        depthImg.setFromPixels(depthImgPixels);
        scanImg.setFromPixels(scanImgPixels);
    
        
    }
    
    
    if (previousKinectTiltAngle != kinectTiltAngle){
        kinect.setCameraTiltAngle(kinectTiltAngle);
    }
    previousKinectTiltAngle = kinectTiltAngle;
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    // draw kinect raw image
    kinect.draw(10, 10, 320, 240);
    kinect.drawDepth(340, 10, 320, 240);
    
    
    // draw delayed image
    if (colorImg.isAllocated()) {
        colorImg.draw(10, 260, 320, 240);
    }
   
    // draw delayed depth
    if (depthImg.isAllocated()) {
        depthImg.draw(340, 260, 320, 240);
    }
    
    // draw scanned img
    if (scanImg.isAllocated()) {
        scanImg.draw(670, 260, 320, 240);
    }
    
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::exit() {
    // close and reset kinect
    kinect.setCameraTiltAngle(0);
    kinect.close();

}
//--------------------------------------------------------------
ofColor ofApp::getPixelColor(int x, int y){
    int frameNumIs = bufferSize - 1;
    ofColor color0;
    
    if (frames.size() > frameNumIs){
        color0 = frames[frameNumIs].getColor(x, y);
    }else{
        color0 = frames[0].getColor(x, y);
    }
    return color0;
}
//--------------------------------------------------------------
ofColor ofApp::getPixelDepth(int x, int y){
    int frameNumIs = bufferSize - 1;
    ofColor depth0;
    
    if (frames.size() > frameNumIs){
        depth0 = depthFrames[frameNumIs].getColor(x, y);
    }else{
        depth0 = depthFrames[0].getColor(x, y);
    }
    return depth0;
}
//--------------------------------------------------------------
ofColor ofApp::getPixelSlitDepthColor(int x, int y){
  
    ofColor color0;
    ofColor depth0;
    int depth1;
    int depth2;
    depth0 = depthFrames[0].getColor(x, y);
    depth1 = abs(depth0.getBrightness());
    depth1 = ofMap(depth1, 0, 255, bufferSize, 0);
    //depth1 = abs(depth1);
    
   // depth2 = abs(ofMap(depth1, 0, 255, bufferSize, 0));
   
    
    

    
    if (depthFrames.size() < bufferSize) {
        color0 = frames[0].getColor(x, y);
    }else{
        depth2 = depthFrames[depth1 - 1].getColor(x, y).getBrightness();
        depth2 = abs(ofMap(depth2, 0, 255, bufferSize, 0));
        
        
        for (int i = 0; i < bufferSize; i++) {
            if (i == depth1) {
                color0 = frames[i].getColor(x, y);
            }
        }
    }
        
    
    return color0;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

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
    bufferSize = 80;
    
    
    imgW = kinect.getWidth();
    imgH = kinect.getHeight();
    
    
    //set gui
    gui.setup();
    gui.add(kinectTiltAngle.set("Kinect Tilt Angle", 0, 0, 30));
    
    depthCVImage.allocate(imgW, imgH);
    depthCVMask.allocate(imgW, imgH);
    depthCVInpainted.allocate(imgW, imgH);

}

//--------------------------------------------------------------
void ofApp::update(){
    // update kinect
    kinect.update();
    
    if (kinect.isFrameNew()){
        
        // inpainting method modified and improved from Marek Berza's ofxKinectInpainter
        depthCVImage.setFromPixels(kinect.getDepthPixels());
        depthCVMask.setFromPixels(kinect.getDepthPixels());
        depthCVMask.threshold(1, true);
        depthCVMask.resize(imgW/DONW_SAMPLING_SCALE, imgH/DONW_SAMPLING_SCALE);
        depthCVImage.resize(imgW/DONW_SAMPLING_SCALE, imgH/DONW_SAMPLING_SCALE);
        depthCVInpainted.resize(imgW/DONW_SAMPLING_SCALE, imgH/DONW_SAMPLING_SCALE);
        
        
        cv::Mat img0 = depthCVImage.getCvImage();
        cv::Mat src = depthCVImage.getCvImage();
        cv::Mat mask = depthCVMask.getCvImage();
        cv::inpaint(src, mask, img0, DEFAULT_INPAINT_RADIUS, cv::INPAINT_TELEA);
        

        depthCVInpainted.scaleIntoMe(depthCVImage, CV_INTER_LINEAR);
        cvCopy(depthCVInpainted.getCvImage(), depthCVImage.getCvImage(), depthCVMask.getCvImage());
        
        depthCVImage.flagImageChanged();
        depthCVImage.resize(imgW, imgH);
        
        ofPixels depth0 = depthCVImage.getPixels();
        depthCVImage.resize(imgW /2, imgH/2);
        
        frames.push_front(kinect.getPixels());
        depthFrames.push_front(depth0);
        
        
        
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
    
    depthCVImage.draw(670, 10);
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

    
    if (depthFrames.size() < bufferSize) {
        color0 = frames[0].getColor(x, y);
    }else{
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

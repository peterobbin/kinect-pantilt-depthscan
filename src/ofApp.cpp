#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // kinect init and open
    kinect.setRegistration(true);
    kinect.init();
    kinect.open();
    
    ofBackground(0);
    ofSetFrameRate(255);
    
    
    // set variables
    bufferSize = 256;
    
    
    imgW = kinect.getWidth();
    imgH = kinect.getHeight();
    
    
    //set gui
    gui.setup();
    gui.add(kinectTiltAngle.set("Kinect Tilt Angle", 0, 0, 30));
    
    depthCVImage.allocate(imgW, imgH);
    depthCVMask.allocate(imgW, imgH);
    depthCVInpainted.allocate(imgW, imgH);
    
    // firmata arduino connection
    ard.connect("/dev/cu.usbserial-DA00VVWF", 57600);
    
    ofAddListener(ard.EInitialized, this, &ofApp::setupArduino);
    bSetupArduino	= false;
    
    horizonalAngle = 90;
    verticalAngle = 95;

}
//--------------------------------------------------------------
void ofApp::setupArduino(const int & version) {
    
    // remove listener because we don't need it anymore
    ofRemoveListener(ard.EInitialized, this, &ofApp::setupArduino);
    
    // it is now safe to send commands to the Arduino
    bSetupArduino = true;
    
    // print firmware name and version to the console
    ofLogNotice() << ard.getFirmwareName();
    ofLogNotice() << "firmata v" << ard.getMajorFirmwareVersion() << "." << ard.getMinorFirmwareVersion();
    
    
    ard.sendServoAttach(9);
    ard.sendServoAttach(10);
    
    
}

//--------------------------------------------------------------
void ofApp::update(){
    // update kinect
    kinect.update();
    
    if (kinect.isFrameNew()){
        
        kinectInpaint();
        
        if(frames.size() > bufferSize){
            frames.pop_back();
        }
        
        if(depthFrames.size() > bufferSize){
            depthFrames.pop_back();
        }
    }
    
    // update arduino
    updateArduino();
    

    
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
void ofApp::updateArduino(){
    ard.update();
}


//--------------------------------------------------------------
void ofApp::draw(){
    
    if (!focusResult){
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
        
        ofDrawBitmapString("raw rgb", 10, 10);
        ofDrawBitmapString("raw depth", 340, 10);
        ofDrawBitmapString("inpainted depth", 670, 10);
        ofDrawBitmapString("raw delayed rgb", 10, 260);
        ofDrawBitmapString("inpainted delayed depth", 340, 260);
        ofDrawBitmapString("realtime slit scan", 670, 260);
    
    }else{
        scanImg.draw(0, 0, 1024, 768);
    }
    
    ofSetColor(255, 255, 255);
    if (!bSetupArduino){
        ofDrawBitmapString("arduino not ready...\n", 525, 40);
    } else {
        ofDrawBitmapString("arduino ready \n", 525, 40);
    }
    
    
    
    
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::kinectInpaint(){
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
    
    //  this line to replace depth image with inpainted one
    ofPixels depth0 = depthCVImage.getPixels();
    
    //  or getting original
    //ofPixels depth0 = kinect.getPixels();
    
    depthCVImage.resize(imgW /2, imgH/2);
    
    frames.push_front(kinect.getPixels());
    depthFrames.push_front(depth0);
    

}

//--------------------------------------------------------------
void ofApp::exit() {
    //  close and reset kinect
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
    // mapping depth to actual buffersize
    // reverse to change refresh order
    //depth1 = ofMap(depth1, 0, 255, 0, bufferSize);
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
    switch(key){
        case ' ':
            focusResult = !focusResult;
        break;
        case OF_KEY_RIGHT:
            // rotate servo head to 180 degrees
            if (horizonalAngle < 180) {
                horizonalAngle ++;
            }
            ard.sendServo(9, horizonalAngle, false);
        break;
        case OF_KEY_LEFT:
            // rotate servo head to 0 degrees
            if (horizonalAngle > 0) {
                horizonalAngle --;
            }
            ard.sendServo(9, horizonalAngle, false);
            
        break;
        case OF_KEY_DOWN:
            // rotate servo head to 0 degrees
            if (verticalAngle < 99) {
                verticalAngle ++;
            }
            ard.sendServo(10, verticalAngle, false);
            
        break;
        case OF_KEY_UP:
            // rotate servo head to 0 degrees
            if (verticalAngle > 85) {
                verticalAngle --;
            }
            ard.sendServo(10, verticalAngle, false);
        
        break;
        case 'r':
            horizonalAngle = 90;
            verticalAngle = 98;
            
            ard.sendServo(9, horizonalAngle, false);
            ard.sendServo(10, verticalAngle, false);
            
            break;
        default:
            break;
    }

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

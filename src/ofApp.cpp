#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    kinect.setRegistration(true);
    kinect.init();
    kinect.open();
    
    ofBackground(0);
}

//--------------------------------------------------------------
void ofApp::update(){
    kinect.update();
    

}

//--------------------------------------------------------------
void ofApp::draw(){
    kinect.draw(10, 10, 320, 240);
    kinect.drawDepth(340, 10, 320, 240);
}

//--------------------------------------------------------------
void ofApp::exit() {
    kinect.setCameraTiltAngle(0); // zero the tilt on exit
    kinect.close();

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

#pragma once

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "opencv2/opencv.hpp"

#define DONW_SAMPLING_SCALE 4
#define DEFAULT_INPAINT_RADIUS 1
class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    
    
    int bufferSize;
    int imgW;
    int imgH;
    
    
    ofxKinect kinect;
    deque<ofPixels> frames;
    deque<ofPixels> depthFrames;
    
    ofPixels colorImgPixels;
    ofImage colorImg;
    
    ofPixels depthImgPixels;
    ofImage depthImg;
    
    ofPixels scanImgPixels;
    ofImage scanImg;
    
    
    ofColor getPixelColor(int x, int y);
    ofColor getPixelDepth(int x, int y);
    ofColor getPixelSlitDepthColor(int x, int y);
    
    ofxCvGrayscaleImage depthCVImage;
    ofxCvGrayscaleImage depthCVMask;
    ofxCvGrayscaleImage depthCVInpainted;
    
    ofxPanel gui;
    ofParameter<int> kinectTiltAngle;
    int previousKinectTiltAngle;
    
    
};

#ifndef _TEST_APP
#define _TEST_APP

//#define USE_IR // Uncomment this to use infra red instead of RGB cam...

#include "ofMain.h"
#include "ofxOpenNI.h"
#include "ofxOpenCv.h"
#include "MyGUI.h"


class testApp : public ofBaseApp{
	
public:
	void setup();
	void update();
	void draw();
	
	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	
	
	
	
	void	setupRecording();
	
	ofShader maskShader;
	
	ofImage masked;	
	ofImage colorImg;	
	
	bool				isLive, isTracking, isRecording, isCloud, isCPBkgnd, isMasking;
	bool				isTrackingHands, isFiltering, masks, drawRef;
	
	ofxOpenNIContext	recordContext, playContext;
	ofxDepthGenerator	recordDepth, playDepth;

#ifdef USE_IR
	ofxIRGenerator		recordImage, playImage;
#else
	ofxImageGenerator	recordImage, playImage; 
#endif
	
//	ofxHandGenerator	recordHandTracker, playHandTracker;
	
	ofxUserGenerator	recordUser, playUser;
	ofxOpenNIRecorder	oniRecorder;
#ifdef TARGET_OSX // only working on Mac at the moment
	ofxHardwareDriver	hardware;
#endif
	void				drawMasks();
	void				drawPointCloud(ofxUserGenerator * user_generator, int userID);
	
	int					nearThreshold, farThreshold;
	int					pointCloudRotationY;
	
	ofImage				allUserMasks, user1Mask, user2Mask, depthRangeMask;
	
	float				filterFactor;
	
	
	ofxCvColorImage			CVcolorImg;
	
	ofxCvGrayscaleImage 	CVgrayImage;
	//ofxCvGrayscaleImage 	CVgrayDepth;
	
	ofxCvGrayscaleImage 	handsImage;
	//ofImage handsImage;
	void updateHandMask();
	
	float handDepth;
	
	
	ofxCvContourFinder 	contourFinder;
	
	unsigned char* tempHand;
	unsigned char* allHands;
	
	ofImage background;
	
	bool smooth;
	bool full;
	
	
	bool showHandsImage;
	
	float leftCrop;
	float bottomCrop;
	MyGUI* gui;
	
	float ratio;
	
	int w, h, x, y;

};

#endif

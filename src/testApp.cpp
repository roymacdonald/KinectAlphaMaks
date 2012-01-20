#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
	
	isLive			= true;
	isTracking		= true;
	isTrackingHands	= false;
	isFiltering		= false;
	isRecording		= false;
	isCPBkgnd		= true;
	isMasking		= true;
	masks			=true;
	drawRef			=true;
	
	smooth = true;
	full = false;
	
	showHandsImage=true;
	
	ofEnableAlphaBlending();
	
	
	ofSetVerticalSync(false);
	
	nearThreshold = 500;
	farThreshold  = 2000;
	
	filterFactor = 0.1f;
	
	setupRecording();
	
	ofBackground(0, 0, 0);
	
	//vidGrabber.setVerbose(true);
	//vidGrabber.initGrabber(640,480);
	
	
	leftCrop =0.0;
	bottomCrop =0.0;
	w=640;
	h=480;
	x=0;
	y=0;
	
	ratio = float(w)/h;
	
	
	handDepth=10;
	
	//---------------------------GUI----------------------------------------------------------------
	
	gui = new MyGUI();
	MyPanel* settingsPanel = new MyPanel("Settings", 0, 500);
	gui->addControl( settingsPanel );
	
	MySlider* leftCropSlider = new MySlider("Left Crop", 10, 30, 200, 20, &leftCrop, 0, 200);
	settingsPanel->addControl( leftCropSlider );
	
	MySlider* bottomCropSlider = new MySlider("Bottom Crop", 10, 60, 200, 20, &bottomCrop, 0, 200);
	settingsPanel->addControl( bottomCropSlider );
	
	MySlider* handDepthSlider = new MySlider("Hand Depth", 10, 90, 200, 20, &handDepth, 0, 100);
	settingsPanel->addControl( handDepthSlider );
	
	//----------------------------------------------------------------------------------------------
	
	
	
	CVcolorImg.allocate(640,480);
	CVgrayImage.allocate(640,480);
	handsImage.allocate(640,480);
	handsImage.setUseTexture(true);
	CVgrayImage.setUseTexture(true);
	//CVgrayDepth.allocate(640,480);
	//CVgrayDepth.setUseTexture(true);

	 //background.loadImage("background.png");
	background.loadImage("underwater.jpg");
	//allUserMasks.allocate(recordUser.getWidth(), recordUser.getHeight(), OF_IMAGE_GRAYSCALE);
	colorImg.allocate(recordImage.getWidth(), recordImage.getHeight(), OF_IMAGE_COLOR);
	
	maskShader.load("composite.vert", "composite.frag");
	maskShader.begin();
	maskShader.setUniformTexture("Tex0", colorImg.getTextureReference(), 0);
	maskShader.setUniformTexture("Tex1", CVgrayImage.getTextureReference(), 1);
	maskShader.end();
	
}

void testApp::setupRecording() {

	hardware.setup();				// libusb direct control of motor, LED and accelerometers
	hardware.setLedOption(LED_OFF); // turn off the led just for yacks (or for live installation/performances ;-)

	recordContext.setup();	// all nodes created by code -> NOT using the xml config file at all
	//recordContext.setupUsingXMLFile();
	recordDepth.setup(&recordContext);
	recordImage.setup(&recordContext);
	
	recordUser.setup(&recordContext);
	recordUser.setSmoothing(filterFactor);				// built in openni skeleton smoothing...
	recordUser.setUseMaskPixels(isMasking);

	
//	recordHandTracker.setup(&recordContext, 4);
//	recordHandTracker.setSmoothing(filterFactor);		// built in openni hand track smoothing...
//	recordHandTracker.setFilterFactors(filterFactor);	// custom smoothing/filtering (can also set per hand with setFilterFactor)...set them all to 0.1f to begin with
	
	recordContext.toggleRegisterViewport();
	recordContext.toggleMirror();
	
	//recordDepth.setDepthColoring(COLORING_GREY_WHITE_MAX);
	
}

void testApp::updateHandMask(){
/*	
	bool firstHandTracked = true;
	int totalHands = recordHandTracker.getNumTrackedHands();
	for(int i=0; i<totalHands; i++){
		ofxTrackedHand*	hand	=recordHandTracker.getHand(i);
		if (hand->isBeingTracked){
			
			hand->progPos;
			if (firstHandTracked) {
				allHands=recordDepth.getDepthPixels(hand->rawPos.Z-handDepth, hand->rawPos.Z+handDepth);
				firstHandTracked=false;
			}else {

			tempHand=recordDepth.getDepthPixels(hand->rawPos.Z-handDepth, hand->rawPos.Z+handDepth);
			
			for (int j=0; j<recordDepth.getWidth()*recordDepth.getHeight(); j++) {
				if (tempHand[j]==255) {
					allHands[j]=255;
				}
			}
			}
			
		}
		
	}
	if(totalHands>0){
	handsImage.setFromPixels(allHands,recordDepth.getWidth(), recordDepth.getHeight());
	}
 */
}


//--------------------------------------------------------------
void testApp::update(){

	hardware.update();
	
	//vidGrabber.update();
	recordContext.update();
	recordDepth.update();
	recordImage.update();
	
	recordUser.update();
		
	//allUserMasks.setFromPixels(recordUser.getUserPixels(), recordUser.getWidth(), recordUser.getHeight(), OF_IMAGE_GRAYSCALE);
	
	CVgrayImage.setFromPixels(recordUser.getUserPixels(), recordUser.getWidth(), recordUser.getHeight());
	//CVgrayDepth.setFromPixels(recordDepth.getUserPixels(), recordDepth.getWidth(), recordDepth.getHeight());
	
	if (smooth) 
	{
	CVgrayImage.dilate();
	CVgrayImage.dilate();
	CVgrayImage.dilate();
	CVgrayImage.erode();
	CVgrayImage.blur(7);
	CVgrayImage.erode();
	CVgrayImage.erode();
	CVgrayImage.blur(7);
}
	CVgrayImage.updateTexture();
	
	
	
	colorImg.setFromPixels(recordImage.getPixels(), recordImage.getWidth(), recordImage.getHeight(), OF_IMAGE_COLOR);
	
	
	contourFinder.findContours(CVgrayImage, 20, (340*240)/3, 10, true);
	
	
	
	ratio=float(640-leftCrop)/(480-bottomCrop);
	
	if (full) {
		
	x=leftCrop;
		h=ofGetHeight();
		w=h*ratio;
		x=float(ofGetWidth()-w)/2;
	}else{
		x=leftCrop;
		w=640-leftCrop;
		h=480-bottomCrop;
	}
	
	
	/*
	int totalHands = recordHandTracker.getNumTrackedHands();
	for(int i=0; i<totalHands; i++){
	ofxTrackedHand*	hand	=recordHandTracker.getHand(i);
		if (hand->isBeingTracked) hand->progPos;
	}*/

	if (showHandsImage) {
		updateHandMask();
	
	}
 
 
}

//--------------------------------------------------------------
void testApp::draw(){
	background.draw(x,y,w,h);
	if (!full) {

	if (drawRef) {
	gui->draw();
	colorImg.draw(320, 500, 320,240);
	recordDepth.draw(640, 0, 320,240);
	CVgrayImage.draw(640, 500, 320,240);
	if (showHandsImage) {
		handsImage.draw(640, 240, 320, 240);
	}

	}
		//recordHandTracker.drawHands();
}
	if (masks) {
		
		
		maskShader.begin();
		
	glActiveTexture(GL_TEXTURE0_ARB);
		colorImg.getTextureReference().bind();
		
		glActiveTexture(GL_TEXTURE1_ARB);
		//allUserMasks.getTextureReference().bind();
		CVgrayImage.getTextureReference().bind();
		glBegin(GL_QUADS);
		
		glMultiTexCoord2d(GL_TEXTURE0_ARB, leftCrop, 0);
		glMultiTexCoord2d(GL_TEXTURE1_ARB, leftCrop, 0);		
		glVertex2f( x, y);
		
		glMultiTexCoord2d(GL_TEXTURE0_ARB, colorImg.getWidth(), 0);
		glMultiTexCoord2d(GL_TEXTURE1_ARB, CVgrayImage.getWidth(), 0);		
		glVertex2f( w, y);
		
		glMultiTexCoord2d(GL_TEXTURE0_ARB, colorImg.getWidth(), colorImg.getHeight()-bottomCrop);
		glMultiTexCoord2d(GL_TEXTURE1_ARB, CVgrayImage.getWidth(), CVgrayImage.getHeight()-bottomCrop);
		glVertex2f(w,h);
		
		glMultiTexCoord2d(GL_TEXTURE0_ARB, leftCrop, colorImg.getHeight()-bottomCrop);
		glMultiTexCoord2d(GL_TEXTURE1_ARB, leftCrop, CVgrayImage.getHeight()-bottomCrop);		
		glVertex2f( x, h);
		
		glEnd();
		
		glActiveTexture(GL_TEXTURE1_ARB);
		CVgrayImage.getTextureReference().unbind();
		
		glActiveTexture(GL_TEXTURE0_ARB);
		colorImg.getTextureReference().unbind();
		
		maskShader.end();
		 
	}
	
	
	//recordHandTracker.drawHands();
	
	
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){
	

	switch (key) {
#ifdef TARGET_OSX // only working on Mac at the moment
		case 357: // up key
			hardware.setTiltAngle(hardware.tilt_angle++);
			break;
		case 359: // down key
			hardware.setTiltAngle(hardware.tilt_angle--);
			break;
#endif
		case 'm':
		case 'M':
			masks =!masks;
			break;
		case 'r':
		case 'R':
			drawRef =!drawRef;
			break;
			
		case 's':
		case 'S':
			smooth = !smooth;
			break;
		case ' ':
			full = !full;
			break;
			
		case 'f':
		case 'F':
			ofToggleFullscreen();
			break;
		case 'h':
		case 'H':
			showHandsImage=!showHandsImage;
			break;
	}	
}


//--------------------------------------------------------------
void testApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	
}


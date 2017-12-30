/************************************************************
************************************************************/
#include "ofApp.h"

/************************************************************
************************************************************/
const char targetIP[] = "10.7.206.7";


/************************************************************
************************************************************/

/******************************
******************************/
ofApp::ofApp()
: Osc_ClapDetector("127.0.0.1", 12348, 12347)
, b_ClapMessage(false)
, State(STATE__WAIT_CLAP)
, Duration(0.6)
{
}

/******************************
******************************/
ofApp::~ofApp()
{
	SendZero_Artnet();
}

/******************************
******************************/
void ofApp::exit(){
	printf("Good bye\n");
}

/******************************
******************************/
void ofApp::SetZero_Artnet(){
	for(int i = 0; i < DMX_SIZE; i++){
		data[i] = 0;
	}
}

/******************************
******************************/
void ofApp::SendZero_Artnet(){
	SetZero_Artnet();
    artnet.sendDmx(targetIP, data, DMX_SIZE);
}

//--------------------------------------------------------------
void ofApp::setup(){
	/********************
	********************/
	ofSetWindowTitle("ANREALAGE:Strobe");
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofSetWindowShape(WIDTH, HEIGHT);
	ofSetEscapeQuitsApp(false);
	
	ofEnableAlphaBlending();
	// ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	
	ofEnableSmoothing();
	
	/********************
	********************/
    //at first you must specify the Ip address of this machine
    artnet.setup("10.0.0.2"); //make sure the firewall is deactivated at this point
	
	for(int i = 0; i < DMX_SIZE; i++){
		data[i] = 0;
	}
	
	/********************
	********************/
	gui.setup();
	{
		ofColor initColor = ofColor(255, 255, 255, 255);
		ofColor minColor = ofColor(0, 0, 0, 0);
		ofColor maxColor = ofColor(255, 255, 255, 255);
		gui.add(StrobeColor.setup("color", initColor, minColor, maxColor));
	}
}

//--------------------------------------------------------------
void ofApp::update(){
	/********************
	********************/
	while(Osc_ClapDetector.OscReceive.hasWaitingMessages()){
		ofxOscMessage m_receive;
		Osc_ClapDetector.OscReceive.getNextMessage(&m_receive);
		
		if(m_receive.getAddress() == "/DetectClap"){
			m_receive.getArgAsInt32(0); // 読み捨て
			
			b_ClapMessage = true;
		}
	}
	
	/********************
	********************/
	StateChart();
	
	/********************
	********************/
	b_ClapMessage = false;
}

/******************************
******************************/
void ofApp::StateChart(){
	/********************
	********************/
	float now = ofGetElapsedTimef();
	
	/********************
	********************/
	switch(State){
		case STATE__WAIT_CLAP:
			if(b_ClapMessage){
				State = STATE__STROBE;
				t_From = now;
			}
			break;
			
		case STATE__STROBE:
			if(b_ClapMessage){
				t_From = now;
			}
			break;
	}
	
	/********************
	********************/
	switch(State){
		case STATE__WAIT_CLAP:
			SetZero_Artnet();
			break;
			
		case STATE__STROBE:
			Progress = (now - t_From) * 100 / Duration;
			if(100 < Progress) Progress = 100;
			
			if(100 <= Progress){
				State = STATE__WAIT_CLAP;
			}
			
			calColor_setDataArray();
			break;
	}
}

/******************************
return
	0 - 1.
******************************/
double ofApp::calLev_Strobe(double Progress){
	if(Progress < 0){
		return 0;
	}else if(100 < Progress){
		return 0;
	}else{
		double tan = -double(1)/100;
		return 1 + tan * Progress;
	}
}

/******************************
******************************/
void ofApp::calColor_setDataArray(){
	/********************
	********************/
	ofColor maxColor = StrobeColor;
	double Lev = calLev_Strobe(Progress);
	
	/********************
	artnetの配列格納は、Ledのch仕様によって異なる.
	********************/
	const int NUM_ITEMS_IN_CH = 6;
	int id = 0;
	
	data[NUM_ITEMS_IN_CH * id + 0] = maxColor.a;
	data[NUM_ITEMS_IN_CH * id + 1] = int(maxColor.r * Lev);
	data[NUM_ITEMS_IN_CH * id + 2] = int(maxColor.g * Lev);
	data[NUM_ITEMS_IN_CH * id + 3] = int(maxColor.b * Lev);
	
	data[NUM_ITEMS_IN_CH * id + 4] = 0; // W
	data[NUM_ITEMS_IN_CH * id + 5] = 1; // Strobe. 1-9:open
	
	/********************
	********************/
	/*
	printf("%5.2f, (%5d, %5d, %5d)\r", Lev, data[NUM_ITEMS_IN_CH * id + 1], data[NUM_ITEMS_IN_CH * id + 2], data[NUM_ITEMS_IN_CH * id + 3]);
	fflush(stdout);
	*/
}

//--------------------------------------------------------------
void ofApp::draw(){
	/********************
	********************/
	ofBackground(30);
	
	/********************
	********************/
    // list nodes for sending
    // with subnet / universe
    artnet.sendDmx(targetIP, data, DMX_SIZE);
	
	/********************
	********************/
	gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch(key){
		case ' ':
			b_ClapMessage = true;
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

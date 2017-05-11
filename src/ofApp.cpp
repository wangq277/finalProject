#include "ofApp.h"

ofPoint p0, p1;		// for line drawing
ofPoint t;		// for translating
float rX, rY, rZ;	// for rotation

ofFbo fbo;
float* fftSmoothed;

ofColor black(0,0,0);


//--------------------------------------------------------------
void ofApp::setup(){
    
    ofBackground(32,12,21);
    ofSetFrameRate(60);
    
    sampleRate = 44100;
    bufferSize = 512;
    int channels = 2;
    
    audioAnalyzer.setup(sampleRate, bufferSize, channels);
    
    player.load("ChinaX.mp3");
    
    fingerMovie.load("giphy.mp4");
    fingerMovie.play();
    
    gui.setup();
    gui.setPosition(20, 150);
    gui.add(smoothing.setup  ("Smoothing", 0.0, 0.0, 1.0));
    
    p0 = ofPoint(-30, 0);
    p1 = ofPoint(30, 0);
    
    t = ofPoint(ofGetWidth()/3, ofGetHeight()/3, 0);
    
    fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA32F_ARB);
    
    fbo.begin();
    ofClear(255,255,255, 0);
    fbo.end();
    
}

float *val;

//--------------------------------------------------------------
void ofApp::update(){
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    //-:Get buffer from sound player:
    soundBuffer = player.getCurrentSoundBuffer(bufferSize);
    
    //-:ANALYZE SOUNDBUFFER:
    audioAnalyzer.analyze(soundBuffer);
    
    
    //-:get Values:
    rms     = audioAnalyzer.getValue(RMS, 0, smoothing);
    power   = audioAnalyzer.getValue(POWER, 0, smoothing);
    pitchFreq = audioAnalyzer.getValue(PITCH_FREQ, 0, smoothing);
    pitchConf = audioAnalyzer.getValue(PITCH_CONFIDENCE, 0, smoothing);
    pitchSalience  = audioAnalyzer.getValue(PITCH_SALIENCE, 0, smoothing);
    inharmonicity   = audioAnalyzer.getValue(INHARMONICITY, 0, smoothing);
    hfc = audioAnalyzer.getValue(HFC, 0, smoothing);
    specComp = audioAnalyzer.getValue(SPECTRAL_COMPLEXITY, 0, smoothing);
    centroid = audioAnalyzer.getValue(CENTROID, 0, smoothing);
    rollOff = audioAnalyzer.getValue(ROLL_OFF, 0, smoothing);
    oddToEven = audioAnalyzer.getValue(ODD_TO_EVEN, 0, smoothing);
    strongPeak = audioAnalyzer.getValue(STRONG_PEAK, 0, smoothing);
    strongDecay = audioAnalyzer.getValue(STRONG_DECAY, 0, smoothing);
    //Normalized values for graphic meters:
    pitchFreqNorm   = audioAnalyzer.getValue(PITCH_FREQ, 0, smoothing, TRUE);
    hfcNorm     = audioAnalyzer.getValue(HFC, 0, smoothing, TRUE);
    specCompNorm = audioAnalyzer.getValue(SPECTRAL_COMPLEXITY, 0, smoothing, TRUE);
    centroidNorm = audioAnalyzer.getValue(CENTROID, 0, smoothing, TRUE);
    rollOffNorm  = audioAnalyzer.getValue(ROLL_OFF, 0, smoothing, TRUE);
    oddToEvenNorm   = audioAnalyzer.getValue(ODD_TO_EVEN, 0, smoothing, TRUE);
    strongPeakNorm  = audioAnalyzer.getValue(STRONG_PEAK, 0, smoothing, TRUE);
    strongDecayNorm = audioAnalyzer.getValue(STRONG_DECAY, 0, smoothing, TRUE);
    
    dissonance = audioAnalyzer.getValue(DISSONANCE, 0, smoothing);
    
    spectrum = audioAnalyzer.getValues(SPECTRUM, 0, smoothing);
    melBands = audioAnalyzer.getValues(MEL_BANDS, 0, smoothing);
    mfcc = audioAnalyzer.getValues(MFCC, 0, smoothing);
    hpcp = audioAnalyzer.getValues(HPCP, 0, smoothing);
    
    tristimulus = audioAnalyzer.getValues(TRISTIMULUS, 0, smoothing);
    
    isOnset = audioAnalyzer.getOnsetValue(0);
  
    fingerMovie.update();
    
    float time = ofGetElapsedTimef();
    
    rX = ofSignedNoise(time * 0.5 + MEL_BANDS * 5) * (150.0 + MEL_BANDS * 5);  // rotate +- 400deg 0.5
    rY = ofSignedNoise(time * 0.3 + MEL_BANDS * 5) * (150.0 + MEL_BANDS * 5);
    rZ = ofSignedNoise(time * 0.9 + MEL_BANDS * 5) * (150.0 + MEL_BANDS * 5); //0.9
    
    //ofPoint dT = ofPoint(ofSignedNoise(time * 0.2) * 3, ofSignedNoise(time * 0.4) * 3, ofSignedNoise(time * 0.6) * 20);
    //t += dT;  // add the delta to our current
    
    float x = ofMap(ofSignedNoise(time * 0.2), -1, 1, 0, ofGetWidth());
    float y = ofMap(ofSignedNoise(time * 0.4), -1, 1, 0, ofGetHeight());
    float z = ofMap(ofSignedNoise(time * 0.2), -1, 1, -500, 500);
    
    t = ofPoint(x, y, z);
    

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    //-Single value Algorithms:
    fingerMovie.draw(300,250,400,400);
    ofPushMatrix();
    ofTranslate(350, 0);
    int mw = 250;
    int xpos = 0;
    int ypos = 30;
    
    float value, valueNorm;
    
    //-Vector Values Algorithms:
    ofTranslate(700, 0);
    
    //img
    
    int graphH = 75;
    int yoffset = graphH + 50;
    ypos = 30;
    
    ofSetColor(255);
     ofDrawBitmapString("Spectrum: ", 0, ypos + 350);
    ofPushMatrix();
    ofTranslate(0, ypos + 300);
    ofSetColor(ofColor::cyan);
    float bin_w = (float) mw / spectrum.size();
    for (int i = 0; i < spectrum.size(); i++){
        float scaledValue = ofMap(spectrum[i], DB_MIN, DB_MAX, 0.0, 1.0, true);//clamped value
        float bin_h = -1 * (scaledValue * graphH);
        ofDrawRectangle(i*bin_w, graphH, bin_w, bin_h);
    }

    ofPopMatrix();
    
    ypos += yoffset;
    ofSetColor(ofRandom(255),ofRandom(255),ofRandom(255));
    ofPushMatrix();
    ofTranslate(0, ypos + 350);
    ofSetColor(ofColor::cyan);
    bin_w = (float) mw / mfcc.size();
    for (int i = 0; i < mfcc.size(); i++){
        float scaledValue = ofMap(mfcc[i], 0, MFCC_MAX_ESTIMATED_VALUE, 0.0, 3.0, true);//clamped value
        float bin_h = -1 * (scaledValue * graphH);
        ofDrawRectangle(i*bin_w , graphH, bin_w, bin_h);
    }

    ofPopMatrix();
    
    //ofPopMatrix();
    
    ypos += yoffset;
    ofSetColor(255);
    //ofDrawBitmapString("Mel Bands: ", 0, ypos);
    ofPushMatrix();
    ofTranslate(750, ypos + 250);
    ofSetColor(ofColor::cyan);
    bin_w = (float) mw / melBands.size();
    for (int i = 0; i < melBands.size(); i++){
        float scaledValue = ofMap(melBands[i], DB_MIN, DB_MAX, 0.0, 1.0, true);//clamped value
        float bin_h = -1 * (scaledValue * graphH);
        ofDrawRectangle(i*bin_w, graphH, bin_w, bin_h);
    }

   
    ofPopMatrix();
    
   // ofPopMatrix();
    
    ypos += yoffset;
    ofSetColor(255);
    //ofDrawBitmapString("HPCP: ", 0, ypos);
    ofPushMatrix();
    ofTranslate(750, ypos - 80);
    ofSetColor(ofColor::cyan);
    bin_w = (float) mw / hpcp.size();
    for (int i = 0; i < hpcp.size(); i++){
        //float scaledValue = ofMap(hpcp[i], DB_MIN, DB_MAX, 0.0, 1.0, true);//clamped value
        float scaledValue = hpcp[i];
        float bin_h = -1 * (scaledValue * graphH);
        ofDrawRectangle(i*bin_w, graphH, bin_w, bin_h);
    }
    ofPopMatrix();

    
    //-Gui & info:
    ofSetColor(255,255,255,80);
    ofNoFill();
    
    fbo.begin();
    ofPushStyle();
    
//    ofSetColor(0,0,0,100);  // alpha fade it
//        ofDrawRectangle(0,0,ofGetWidth(),ofGetHeight());
    
    ofPopStyle();
    
    
    ofPushMatrix();
    ofSetColor(0,0,0,100);  // alpha fade it
    ofDrawRectangle(0,0,ofGetWidth(),ofGetHeight());
    
    ofTranslate(t);
    
    ofRotateX(rX);
    ofRotateY(rY);
    ofRotateZ(rZ);
    
    ofPushStyle();
    
    for(int i = 0; i< 20;i++){
        ofSetColor(0, 0, 255);
        ofDrawLine(p0 , p1);
    }
    ofPopStyle();
    ofPopMatrix();
    fbo.end();
    
    fbo.draw(0,0);
    //backIamge.draw(0,0);
    // string msg = ofToString((int) ofGetFrameRate()) + " fps";
    

 
    gui.draw();
    ofSetColor(255);
    //ofDrawBitmapString("ofxAudioAnalyzer\n\nALL ALGORITHMS EXAMPLE", 10, 32);
    ofSetColor(ofColor::hotPink);
    ofDrawBitmapString("Keys 1-6: Play audio tracks", 10, 50);
    

    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    player.stop();
    switch (key) {
       
        case '1':
            player.load("beforeISleep.mp3");
            break;
        case '2':
            player.load("Chinese.mp3");
            break;
        case '3':
            player.load("ChinaX.mp3");
            break;
        case '4':
            player.load("horizon.mp3");
            break;
        case '5':
            player.load("PDD.mp3");
            break;
        case '6':
            player.load("ChinaP.mp3");
            break;
            
            
        default:
            break;
    }
    player.play();
    
}
//--------------------------------------------------------------
void ofApp::exit(){
    audioAnalyzer.exit();
    player.stop();
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

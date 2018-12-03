#include "ofApp.h"

//Listen to localhost
//#define HOST "10.0.0.58"
#define HOST "192.168.2.58"

//create osc message object
ofxOscMessage m;
string incomingAddr;
float incomingVal;
float msg[4];
long int timer;
int toggle = 0;

int track = 5;
//--------------------------------------------------------------
//----------------------  Params -------------------------------
//--------------------------------------------------------------
Params param;        //Definition of global variable

void Params::setup() {
    eCenter = ofPoint( ofGetWidth() / 2, ofGetHeight() / 2 );
    eRad = 600;
    velRad = 200;
    lifeTime = 5.0;
    rotate = 140;
    
    force = 0;
    spinning = 100;
    friction = 0.2;
    
    eCenter2 = ofPoint( ofGetWidth() / 2, ofGetHeight() / 2 );
    eRad2 = 12000;
    velRad2 = 100;
    lifeTime2 = 10.0;
    rotate2 = 100;
    
    force2 = 1000;
    spinning2 = 100;
    friction2 = 0.1;
    
}

//--------------------------------------------------------------
//----------------------  Particle  ----------------------------
//--------------------------------------------------------------
Particle::Particle() {
    live = false;
    live2 = false;
}

//--------------------------------------------------------------
ofPoint randomPointInCircle( float maxRad ){
    ofPoint pnt;
    float rad = ofRandom( 0, maxRad );
    float angle = ofRandom( 0, M_TWO_PI );
    pnt.x = cos( angle ) * rad;
    pnt.y = sin( angle ) * rad;
    return pnt;
}

//--------------------------------------------------------------
void Particle::setup() {
    pos = param.eCenter + randomPointInCircle( param.eRad );
    vel = randomPointInCircle( param.velRad );
    time = 0;
    lifeTime = param.lifeTime;
    live = true;
    
    //shooting stars
    pos2 = param.eCenter2 + randomPointInCircle( param.eRad2 );
    vel2 = randomPointInCircle( param.velRad2 );
    time2 = 0;
    lifeTime2 = param.lifeTime2;
    live2 = true;
    
    peak3.load("peak v2B.mp3");
    peak3.setVolume(0.2);
}

//--------------------------------------------------------------
void Particle::update( float dt ){
    ofSoundUpdate();
    if ( live ) {
        //        Rotate vel
        
        vel.rotate( 0, 0, param.rotate * dt );
        
        ofPoint acc;         //Acceleration
        ofPoint delta = pos - param.eCenter;
        float len = delta.length();
        if ( ofInRange( len, 0, param.eRad ) ) {
            delta.normalize();
            
            //Attraction/repulsion force
            acc += delta * param.force;
            
            //Spinning force
            acc.x += -delta.y * param.spinning;
            acc.y += delta.x * param.spinning;
        }
        vel += acc * dt;            //Euler method
        vel *= (1-param.friction);  //Friction
        
        //Update pos
        pos += vel * dt;    //Euler method
        
        //Update time and check if particle should die
        time += dt;
        if ( time >= lifeTime ) {
            live = false;   //Particle is now considered as died
        }
    }
    if ( live2 ) {
        //Rotate vel2 for shooting stars
        vel2.rotate( 0, 0, param.rotate2 * dt );
        
        ofPoint acc2;         //Acceleration
        ofPoint delta2 = pos2 - param.eCenter2;
        float len2 = delta2.length();
        if ( ofInRange( len2, 0, param.eRad2 ) ) {
            delta2.normalize();
            
            //Attraction/repulsion force
            acc2 += delta2 * param.force2;
            
            //Spinning force
            acc2.x += -delta2.y * param.spinning2;
            acc2.y += delta2.x * param.spinning2;
        }
        vel2 += acc2 * dt;            //Euler method
        vel2 *= (1-param.friction2);  //Friction
        
        //Update pos
        pos2 += vel2 * dt;    //Euler method
        
        //Update time and check if particle should die
        time2 += dt;
        if ( time2 >= lifeTime2 ) {
            live2 = false;   //Particle is now died
        }
    }
}

//--------------------------------------------------------------
void Particle::draw(){
    if ( live ) {
        //Compute size
        float size = ofMap(
                           fabs(time - lifeTime/2), 0, lifeTime/2, 1, 0 );
        
        //Compute color
        ofColor color = ofColor::yellow;
        float hue = ofMap( time, 0, lifeTime, 110, 170 );
        color.setHue( hue );
        ofSetColor( color );
        
        ofDrawCircle( pos, size );  //Draw particle
    }
}
void Particle::draw2(){
    //    peak3.play();
    if ( live2 ) {
        //Compute size
        float size = ofMap(
                           fabs(time - lifeTime2/2), 0, lifeTime2/2, 0, 1 );
        
        //Compute color
        ofColor color = ofColor::white;
        float hue = ofMap( time, 0, lifeTime, 10, 11 );
        color.setHue( hue );
        ofSetColor( color );
        
        ofDrawCircle( pos2, size );  //Draw particle
        
    }
}

//--------------------------------------------------------------
//----------------------  ofApp  -----------------------------
//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate( 60 );    //Set screen frame rate
    receiver.setup(5000);
    
    //Allocate drawing buffer
    int w = ofGetWidth();
    int h = ofGetHeight();
    fbo.allocate( w, h, GL_RGB32F_ARB );
    
    //Fill buffer with white color
    fbo.begin();
    ofBackground(0);
    fbo.end();
    
    //Set up parameters
    param.setup();        //Global parameters
    history = 10.0;
    bornRate = 50;
    bornCount = 10;
    
    time0 = ofGetElapsedTimef();
    history2 = 0.001;
    bornRate2 = 0.01;
    bornCount2 = 10;
    //    bgm.setMultiPlay(true);
    //    bgm.load("Nostalgia v2.mp3");
    //    bgm.setLoop(true);
    //    bgm.play();
    //    thread2.startThread();
    
}

//--------------------------------------------------------------
void ofApp::update(){
    ofSoundUpdate();
    while(receiver.getNextMessage(m)) {
        if (m.getAddress() == "museData /elements/beta_absolute"){
            incomingAddr = m.getAddress();
            //loop through the incoming arguments check their type and conver to string as needed
            for(int i = 0; i < m.getNumArgs(); i++){
                if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
                    incomingVal = m.getArgAsFloat(i);
                    msg[i] = incomingVal;
                }
                ofLog(OF_LOG_NOTICE, "beta 1: " + ofToString(msg[3]) + ", beta 2: " + ofToString(msg[1]) + ", beta 3: " + ofToString(msg[2]) + ", beta 4: " + ofToString(msg[3]));
            }
        }
    }
    
    //Compute dt
    float time = ofGetElapsedTimef();
    float dt = ofClamp( time - time0, 0, 0.1 );
    time0 = time;
    
    //Delete inactive particles
    int i=0;
    while (i < p.size()) {
        if ( !p[i].live ) {
            p.erase( p.begin() + i );
        }
        else {
            i++;
        }
    }
    
    //Born new particles
    bornCount += dt * bornRate;      //Update bornCount value
    if ( bornCount >= 1 ) {          //It's time to born particle(s)
        int bornN = int( bornCount );//How many born
        bornCount -= bornN;          //Correct bornCount value
        for (int i=0; i<bornN; i++) {
            Particle newP;
            newP.setup();            //Start a new particle
            p.push_back( newP );     //Add this particle to array
        }
    }
    
    //Update the particles
    for (int i=0; i<p.size(); i++) {
        p[i].update( dt );
    }
    
    bornCount2 += dt * bornRate2;      //Update bornCount value
    if ( bornCount2 >= 1 ) {          //It's time to born particle(s)
        int bornN2 = int( bornCount2 );//How many born
        bornCount2 -= bornN2;          //Correct bornCount value
        for (int i=0; i<bornN2; i++) {
            Particle newP2;
            newP2.setup();            //Start a new particle
            p.push_back( newP2 );     //Add this particle to array
        }
    }
    
    //Update the particles
    for (int i=0; i<p.size(); i++) {
        p[i].update( dt );
    }
    
    //        thread.lock();
    if (msg[0] > 0.6) {
        if (toggle == 0){
            thread.startThread();
            thread2.startThread();
        }
    }
    //        thread.unlock();
    
    
}

void SoundThread::threadedFunction() {
    
    peak1.load("peak 1.wav");
    peak1.setVolume(0.2);
    peak1.setMultiPlay(true);
    peak2.load("peak 2A.wav");
    peak2.setVolume(0.2);
    peak2.setMultiPlay(true);
    peak3.load("peak 2B.wav");
    peak3.setVolume(0.2);
    peak3.setMultiPlay(true);
    peak4.load("peak 3A.wav");
    peak4.setVolume(0.2);
    peak4.setMultiPlay(true);
    peak5.load("peak 3B.wav");
    peak5.setVolume(0.2);
    peak5.setMultiPlay(true);
    peak6.load("peak 3C.wav");
    peak6.setVolume(0.2);
    peak6.setMultiPlay(true);
    peak7.load("peak 3D.wav");
    peak7.setVolume(0.2);
    peak7.setMultiPlay(true);
    while(isThreadRunning() && toggle == 0) {
        lock();
        switch (track) {
            case 1:
                peak1.play();
                track = 2;
                break;
            case 2:
                peak2.play();
                track = 3;
                break;
            case 3:
                peak3.play();
                track = 2;
                break;
            case 4:
                peak1.play();
                track = 5;
                break;
            case 5:
                peak2.play();
                track = 6;
                break;
            case 6:
                peak3.play();
                track = 5;
                break;
        }
        toggle=1;
        unlock();
    }
    
}

void ToggleThread::threadedFunction() {
    timer = (int)ofGetElapsedTimef();
    while((int)ofGetElapsedTimef() - timer < 10 ){
    }
    toggle=0;
    timer=0;
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground( 255);  //Set white background
    //    thread.lock();
    //    if (msg[0] >0.5) {
    //        if (toggle == 0){
    //            thread.startThread();
    //            thread2.startThread();
    //        }
    //    }
    //    thread.unlock();
    //1. Drawing to buffer
    fbo.begin();
    
    //Draw semi-transparent white rectangle
    //to slightly clearing a buffer (depends on history value)
    
    ofEnableAlphaBlending();         //Enable transparency
    
    float alpha = (1-history) * 255;
    ofSetColor( 255, 255, 255, alpha );
    ofFill();
    ofRectangle( 0, 0, ofGetWidth(), ofGetHeight() );
    
    ofDisableAlphaBlending();        //Disable transparency
    
    //Draw the particles
    ofFill();
    //    int toggle =0;
    for (int i=0; i<p.size(); i++) {
        if (msg[0] > 0.6) {
            //            if (toggle == 0){
            p[i].draw2();
            //            }
        }
        //        else {
        p[i].draw();
        //        }
    }
    fbo.end();
    //2. Draw buffer on the screen
    ofSetColor( 255, 255, 255 );
    fbo.draw( 0, 0 );
    
    ofDrawBitmapString("beta 1: " + ofToString(msg[0]), 4, 30);
    ofDrawBitmapString("beta 3: " + ofToString(msg[2]), 4, 40);
    ofDrawBitmapString("beta 2: " + ofToString(msg[1]), 4, 50);
    ofDrawBitmapString("beta 4: " + ofToString(msg[3]), 4, 60);
    
    ofDrawBitmapString("timer: " + ofToString(timer), 4, 70);
    ofDrawBitmapString("toggle: " + ofToString(toggle), 4, 80);
    ofDrawBitmapString("elapsed time: " + ofToString((int)ofGetElapsedTimef()), 4, 90);
}

void ofApp::exit() {
    // stop the thread
    thread.stopThread();
}

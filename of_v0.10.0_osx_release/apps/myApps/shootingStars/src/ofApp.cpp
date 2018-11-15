#include "ofApp.h"

//Listen to localhost
#define HOST "192.168.2.84"

//create osc message object
ofxOscMessage m;
string incomingAddr;
float incomingVal;
float msg[4];
int toggle = 0;

//--------------------------------------------------------------
//----------------------  Params -------------------------------
//--------------------------------------------------------------
Params param;        //Definition of global variable

void Params::setup() {
    eCenter = ofPoint( ofGetWidth() / 2, ofGetHeight() / 2 );
    eRad = 1000;
    velRad = 1000;
    lifeTime = 10.0;
    rotate = 120;
    
    force = 100;
    spinning = 100;
    friction = 0.15;
    
    eCenter2 = ofPoint( ofGetWidth() / 2, ofGetHeight() / 2 );
    eRad2 = 20000;
    velRad2 = 300;
    lifeTime2 = 1.0;
    rotate2 = 2;
    
    force2 = 0;
    spinning2 = 0;
    friction2 = 0.001;
    
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
//        if (msg[0] > 0.4) {
//            peak1.play();
//        }
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
            live2 = false;   //Particle is now considered as died
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
        ofColor color = ofColor::red;
        float hue = ofMap( time, 0, lifeTime, 180, 255 );
        color.setHue( hue );
        ofSetColor( color );
        
        ofDrawCircle( pos, size );  //Draw particle
    }
}
void Particle::draw2(){
    peak3.play();
    if ( live2 ) {
        //Compute size
        float size = ofMap(
                           fabs(time - lifeTime2/2), 0, lifeTime2/2, 1, 0 );
        
        //Compute color
        ofColor color = ofColor::white;
        float hue = ofMap( time, 0, lifeTime, 180, 255 );
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
    bornRate = 100;
    bornCount = 10;
    
    time0 = ofGetElapsedTimef();
    history2 = 10.0;
    bornRate2 = 1;
    bornCount2 = 1;
    bgm.load("Nostalgia v2.mp3");
    bgm.setLoop(true);
    bgm.play();
    
    peak1.load("peak v1.mp3");
    peak1.setVolume(0.2);
    peak2.load("peak v2A.mp3");
    peak3.load("peak v2B.mp3");
    peak3.setVolume(0.2);
    
    valley1.load("valley v1.mp3");
    valley2.load("valley v2A.mp3");
    valley3.load("valley v2B.mp3");
    
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
//    if (msg[0] > 0.4) {
//        peak1.play();
//    }
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
    

}
void ofApp::playPeak3(){
    peak3.play();
}
//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground( 255);  //Set white background
    
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
    for (int i=0; i<p.size(); i++) {
        p[i].draw();
        if (msg[0] > 0.4) {
            p[i].draw2();
        }
    }
    
    fbo.end();
    
    //2. Draw buffer on the screen
    ofSetColor( 255, 255, 255 );
    fbo.draw( 0, 0 );
    
    ofDrawBitmapString("beta 1: " + ofToString(msg[0]), 0, 30);
    ofDrawBitmapString("beta 2: " + ofToString(msg[1]), 0, 40);
    ofDrawBitmapString("beta 3: " + ofToString(msg[2]), 0, 50);
    ofDrawBitmapString("beta 4: " + ofToString(msg[3]), 0, 60);
}

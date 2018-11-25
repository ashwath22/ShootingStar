#pragma once
#include "ofMain.h"
#include "ofxOsc.h"

//Particle class
class Particle {
public:
    Particle();                //Class constructor
    void setup();              //Start particle
    void update( float dt );   //Recalculate physics
    void draw();//Draw particle
    void setup2();              //Start particle
    void update2( float dt );   //Recalculate physics
    void draw2();               //Draw particle
    
    ofPoint pos;               //Position
    ofPoint vel;               //Velocity
    
    ofPoint pos2;               //Position
    ofPoint vel2;               //Velocity
    
    float time;                //Time of living
    float lifeTime;            //Allowed lifetime
    bool live;                 //Is particle live
    
    float time2;                //Time of living
    float lifeTime2;            //Allowed lifetime
    bool live2;                 //Is particle live
    
    ofSoundPlayer peak1;
    ofSoundPlayer peak3;
};

//Control parameters class
class Params {
public:
    void setup();
    ofPoint eCenter;    //Emitter center
    float eRad;         //Emitter radius
    float velRad;       //Initial velocity limit
    float lifeTime;     //Lifetime in seconds
    
    float rotate;   //Direction rotation speed in angles per second
    
    float force;       //Attraction/repulsion force inside emitter
    float spinning;    //Spinning force inside emitter
    float friction;    //Friction, in the range [0, 1]
    
    //shooting star
    ofPoint eCenter2;    //Emitter center
    float eRad2;         //Emitter radius
    float velRad2;       //Initial velocity limit
    float lifeTime2;     //Lifetime in seconds
    
    float rotate2;   //Direction rotation speed in angles per second
    
    float force2;       //Attraction/repulsion force inside emitter
    float spinning2;    //Spinning force inside emitter
    float friction2;    //Friction, in the range [0, 1]
};

extern Params params; //Declaration a of global variable

class SoundThread : public ofThread {
    // the thread function
    void threadedFunction();
    ofSoundPlayer peak3;
};

class ToggleThread : public ofThread {
    // the thread function
    void threadedFunction();
};

//openFrameworks' class
class ofApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    void playPeak3();
    
    vector<Particle> p;      //Particles
    ofFbo fbo;            //Off-screen buffer for trails
    
    float history;        //Control parameter for trails
    float time0;          //Time value for computing dt
    float history2;        //Control parameter for trails
    float time02;          //Time value for computing dt
    
    float bornRate;       //Particles born rate per second
    float bornCount;      //Integrated number of particles to born
    float bornRate2;       //Particles born rate per second
    float bornCount2;      //Integrated number of particles to born
    
    ofxOscReceiver receiver;
    ofSoundPlayer bgm;
    ofSoundPlayer peak1;
    ofSoundPlayer peak2;
    ofSoundPlayer peak3;
    ofSoundPlayer valley1;
    ofSoundPlayer valley2;
    ofSoundPlayer valley3;
    
    SoundThread thread;
    ToggleThread thread2;
};

//
//  ofxRayComposer.cpp
//  ofxIlda example basic
//
//  Created by Jonas Fehr on 05/04/16.
//
//

#include "ofxRayComposer.h"

#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "rcdev.h"








//--------------------------------------------------------------
void ofxRayComposer::setup(bool bStartThread, int idRayComposer) {
    
  

    
    
    
    idRayComposerConnection = idRayComposer;
    
   // etherdream_lib_start();
    
    setPPS(30000);
    setWaitBeforeSend(false);
    
    /* Sleep for a bit over a second, to ensure that we see broadcasts
     * from all available DACs. */
    usleep(1000000);
    
    init();
    
    if(bStartThread) start();
}


//--------------------------------------------------------------
bool ofxRayComposer::stateIsFound() {
    return state == RAYCOMPOSER_FOUND;
}

//--------------------------------------------------------------
void ofxRayComposer::init() {
    /* Initialise the Library. API Version is returned. */
    int ret = RCInit();
    if(ret < 0){
        ofLogWarning() << "ofxRayComposer::init - Error initialising Library! Exit.\n";
        return -1;
    }
    if(ret < RCAPI_VERSION){
        ofLogWarning() << "ofxRayComposer::init - API Version too old. Please use a newer dll/lib.\n";
        return -2;
    }
    ofLogWarning() << "ofxRayComposer::init - API Version " << (ret >> 8) << "."<< (ret & 0xFF) << "\n"; //printf("API Version %d.%02d\n", ret >> 8, ret & 0xFF);
    
    /* Detect & enumerate the connected devices. */
    count = RCEnumerateDevices();
    if(count < 0){
        ofLogWarning() << "ofxRayComposer::init - Error enumerating devices! Exit.\n";
        return -3;
    }
    if(count == 0){
        ofLogWarning() << "ofxRayComposer::init - No devices found. Exit.\n";
        return 0;
    }
    
    /* List the devices found. */
    printf("Found %d device(s): \n", count);
    for(int i = 0; i < count; i++){
        ret = RCDeviceID(i, deviceId, sizeof(deviceId));
        if(ret < 0){
            ofLogWarning() << "ofxRayComposer::init - Error reading device id! Exit.\n";
            return -4;
        }
        ofLogWarning() << "ofxRayComposer::init - device " << i << " id " << deviceId << "\n"; //printf(" %d: %s\n", i, deviceId);
    }
    
    /* Demo laser output */
    /* Select first device in list. */
    ret = RCDeviceID(idRayComposerConnection, deviceId, sizeof(deviceId));
    if(ret < 0){
        ofLogWarning() << "ofxRayComposer::init - Error reading device id! Exit.\n";
        return -5;
    }
    ofLogWarning() << "ofxRayComposer::init - Opening device: " <<  deviceId << "\n";
    handle = RCOpenDevice(deviceId);
    if(handle < 0){
        ofLogWarning() << "ofxRayComposer::init - Error opening device: " << handle << "! Exit.\n";
        return -6;
    }
    ofLogWarning() << "ofxRayComposer::init - Starting laser.\n";
    ret = RCStartOutput(handle);
    if(ret < RCOk){
        ofLogWarning() << "ofxRayComposer::init - Error starting laser output: " << ret << "! Exit.\n";
        return -7;
    }
    
    ofLogNotice() << "ofxRayComposer::init - done";

    state = RAYCOMPOSER_FOUND;
}

//--------------------------------------------------------------
void ofxRayComposer::kill() {
    clear();
    stop();
    if(stateIsFound()) {
        printf("\nStoping laser.\n");
        int ret = RCStopOutput(handle);
        if(ret < RCOk){
            ofLogWarning() << "ofxRayComposer::kill - Error stoping laser output: " << ret << "! Exit.\n";
            //return -10;
        }
        printf("Closing device.\n");
        ret = RCCloseDevice(handle);
        if(ret < RCOk){
            ofLogWarning() << "ofxRayComposer::kill - Error closing device: " << ret << "! Exit.\n";
            //return -11;
        }
        ret = RCExit();
        if(ret < RCOk){
            ofLogWarning() << "ofxRayComposer::kill - Error closing Library: " << ret << "! Exit.\n";
            //return -12;
        }
    }
}

//--------------------------------------------------------------
void ofxRayComposer::threadedFunction() {
    while (isThreadRunning() != 0) {
        
        switch (state) {
            case RAYCOMPOSER_NOTFOUND:
                if(bAutoConnect) init();
                break;
                
            case RAYCOMPOSER_FOUND:
                if(lock()) {
                    send();
                    unlock();
                }
                break;
        }
    }
}

//--------------------------------------------------------------
void ofxRayComposer::start() {
    startThread(true);  // TODO: blocking or nonblocking?
}

//--------------------------------------------------------------
void ofxRayComposer::stop() {
    stopThread();
}

//--------------------------------------------------------------
void ofxRayComposer::send() {
    if(!stateIsFound() || points.empty()) return;
    
//    if(bWaitBeforeSend) etherdream_wait_for_ready(device);
//    else if(!etherdream_is_ready(device)) return;
//    
//    // DODGY HACK: casting ofxIlda::Point* to etherdream_point*
//    int res = etherdream_write(device, (etherdream_point*)points.data(), points.size(), pps, 1);
//    if (res != 0) {
//        ofLogVerbose() << "ofxRayComposer::write " << res;
//    }
    
        /* Your TODO: create fancy laser graphics here */
//
//    struct RCPoint {
//        /** X (horizontal) position signal; range -32768 to 32767 results in -10V to +10V on the ILDA connector */
//        signed short x;
//        /** Y (vertical) position signal; range -32768 to 32767 results in -10V to +10V on the ILDA connector */
//        signed short y;
//        /** Red color signal; range 0 to 65535 results in 0V to +5V on the ILDA connector */
//        unsigned short red;
//        /** Green color signal; range 0 to 65535 results in 0V to +5V on the ILDA connector */
//        unsigned short green;
//        /** Blue color signal; range 0 to 65535 results in 0V to +5V on the ILDA connector */
//        unsigned short blue;
//        /** Intensity signal, range 0 to 65535 results in 0V to +5V on the ILDA connector */
//        unsigned short intensity;
//        /** User 1 (Cyan) signal; range 0 to 65535 results in 0V to +5V on the ILDA connector */
//        unsigned short user1;
//        /** User 2 (Magenta) signal; range 0 to 65535 results in 0V to +5V on the ILDA connector */
//        unsigned short user2;
//    };
    
    //    vector<RCPoint> rcPoints;
    struct RCPoint rcPoints[2000];
        for(int i=0; i < points.size(); i++){
            //rcPoints.push_back(RCPoint());

            rcPoints[i].x = points[i].x;
            rcPoints[i].y = points[i].y;
            rcPoints[i].red = points[i].r;
            rcPoints[i].green = points[i].g;
            rcPoints[i].blue = points[i].b;
            rcPoints[i].intensity = points[i].a;
            rcPoints[i].user1 = points[i].u1;
            rcPoints[i].user2 = points[i].u2;
        }
        /* wait for free buffer; second parameter is timeout
         *   0 = poll number of free buffers only, return immediately
         * < 0 = wait forever until buffer becomes free
         * > 0 = wait the number of miliseconds or until a buffer becomes free */
        int ret = RCWaitForReady(handle, -1);
        if(ret < RCOk){
            ofLogWarning() << "ofxRayComposer::send - Error waiting for free buffer: " << ret << "! Exit.\n";
            //return -8;
        }
    
        unsigned int pointCount = sizeof(points) / sizeof(struct RCPoint);
        ret = RCWriteFrame(handle, rcPoints, pointCount, pps, 0);
        
        if(ret < RCOk){
            ofLogWarning() << "ofxRayComposer::send - Error writing frame to device: " << ret << "! Exit.\n";
            //return -9;
        }
        points.clear();
}


//--------------------------------------------------------------
void ofxRayComposer::clear() {
    if(lock()) {
        points.clear();
        unlock();
    }
}

//--------------------------------------------------------------
void ofxRayComposer::addPoints(const vector<ofxIlda::Point>& _points) {
    if(lock()) {
        if(!_points.empty()) {
            points.insert(points.end(), _points.begin(), _points.end());
        }
        unlock();
    }
}


//--------------------------------------------------------------
void ofxRayComposer::addPoints(const ofxIlda::Frame &ildaFrame) {
    addPoints(ildaFrame.getPoints());
}


//--------------------------------------------------------------
void ofxRayComposer::setPoints(const vector<ofxIlda::Point>& _points) {
    if(lock()) {
        points = _points;
        unlock();
    }
}


//--------------------------------------------------------------
void ofxRayComposer::setPoints(const ofxIlda::Frame &ildaFrame) {
    setPoints(ildaFrame.getPoints());
}

//--------------------------------------------------------------
void ofxRayComposer::setWaitBeforeSend(bool b) {
    if(lock()) {
        bWaitBeforeSend = b;
        unlock();
    }
}

//--------------------------------------------------------------
bool ofxRayComposer::getWaitBeforeSend() const {
    return bWaitBeforeSend;
}


//--------------------------------------------------------------
void ofxRayComposer::setPPS(int i) {
    if(lock()) {
        pps = i;
        unlock();
    }
}

//--------------------------------------------------------------
int ofxRayComposer::getPPS() const {
    return pps;
}


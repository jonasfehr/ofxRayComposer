//
//  ofxRayComposer.cpp
//  ofxIlda example basic
//
//  Created by Jonas Fehr on 05/04/16.
//
//

#include "ofxRayComposer.h"

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
    ofLogNotice() << "ofxRayComposer::init - API Version " << (ret >> 8) << "."<< (ret & 0xFF) << "\n"; //printf("API Version %d.%02d\n", ret >> 8, ret & 0xFF);
    
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
        ofLogNotice() << "ofxRayComposer::init - device " << i << " - id: " << deviceId << "\n"; //printf(" %d: %s\n", i, deviceId);
    }
    
    /* Demo laser output */
    /* Select first device in list. */
    ret = RCDeviceID(idRayComposerConnection, deviceId, sizeof(deviceId));
    if(ret < 0){
        ofLogWarning() << "ofxRayComposer::init - Error reading device id! Exit.\n";
        return -5;
    }
    ofLogNotice() << "ofxRayComposer::init - Opening device: " <<  deviceId << "\n";
    handle = RCOpenDevice(deviceId);
    if(handle < 0){
        ofLogWarning() << "ofxRayComposer::init - Error opening device: " << handle << "! Exit.\n";
        return -6;
    }
    ofLogNotice() << "ofxRayComposer::init - Starting laser.\n";
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
    ofLogNotice() << "ofxRayComposer::kill - done";
    
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
    int ret = RCWaitForReady(handle, -1);
    if(ret < RCOk){
        ofLogWarning() << "ofxRayComposer::send - Error waiting for free buffer: " << ret << "! Exit.\n";
        //return -8;
    }
    ret = RCWriteFrame(handle, (RCPoint*)points.data(), points.size(), pps, 0);
    
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


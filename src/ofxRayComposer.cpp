//
//  ofxRayComposer.cpp
//  ofxIlda example basic
//
//  Created by Jonas Fehr on 05/04/16.
//
//

#include "ofxRayComposer.h"

//--------------------------------------------------------------
void ofxRayComposer::setup(bool bStartThread, int idRayComposer, bool extraSafety) {
    
    idRayComposerConnection = idRayComposer;
    bExtraSafety = extraSafety;
    
    // etherdream_lib_start();
    
    setPPS(30000);
    setWaitBeforeSend(false);
    
    /* Sleep for a bit over a second, to ensure that we see broadcasts
     * from all available DACs. */
    usleep(1000000);
    
    init();
    
    if(bStartThread && stateIsFound()) start();
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
                if(lock()){// if of 0.10 tryLock()) {
                    if(bExtraSafety){
                        if(checkLastUpdate()) send();
                        else sendBlack();
                    }
                    else send();
                    unlock();
                }
                break;
        }
    }
}

//--------------------------------------------------------------
void ofxRayComposer::start() {
    startThread();
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
void ofxRayComposer::sendBlack() {
        points.clear();
        ofxIlda::PointDac p;
        p.x = 0;
        p.y = 0;
        p.r = p.g = p.b = p.u1 = p.u2 = 0;
        points.push_back(p);
    send();
}


//--------------------------------------------------------------
void ofxRayComposer::clear() {
    if(lock()) {
        points.clear();
        unlock();
    }
}

//--------------------------------------------------------------
void ofxRayComposer::addPoints(const vector<ofxIlda::PointDac>& _points) {
    if(lock()) {
        if(!_points.empty()) {
            points.insert(points.end(), _points.begin(), _points.end());
        }
        unlock();
    }
    updated();
}


//--------------------------------------------------------------
void ofxRayComposer::addPoints(const ofxIlda::Frame &ildaFrame) {
    addPoints(ildaFrame.getPointsDac());
}


//--------------------------------------------------------------
void ofxRayComposer::setPoints(const vector<ofxIlda::PointDac>& _points) {
    if(lock()) {
        points = _points;
        unlock();
    }
    updated();
}


//--------------------------------------------------------------
void ofxRayComposer::setPoints(const ofxIlda::Frame &ildaFrame) {
    setPoints(ildaFrame.getPointsDac());
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

//--------------------------------------------------------------
void ofxRayComposer::updated(){
    lastUpdate = ofGetElapsedTimef();
}

//--------------------------------------------------------------
bool ofxRayComposer::checkLastUpdate(){
    return ofGetElapsedTimef()-lastUpdate < MAX_TIME_SAFE;
}

//--------------------------------------------------------------
// for compability with ofxLaser
//--------------------------------------------------------------
bool ofxRayComposer::sendFrame(const vector<ofxLaser::Point>& points){
        vector<ofxIlda::PointDac> sendpoints(points.size());
        for(int i = 0; i<points.size(); i++) {
            ofxIlda::PointDac& p1 = sendpoints[i];
            ofxLaser::Point p2 = points[i];
            p1.x = ofMap(p2.x,0,800,kIldaMinPoint, kIldaMaxPoint);
            p1.y = ofMap(p2.y,800,0,kIldaMinPoint, kIldaMaxPoint); // Y is UP in ilda specs
            p1.r = p2.r/255.0f*kIldaMaxIntensity;
            p1.g = p2.g/255.0f*kIldaMaxIntensity;
            p1.b = p2.b/255.0f*kIldaMaxIntensity;
//            p1.i = 0;
            p1.u1 = 0;
            p1.u2 = 0;
        }
        addPoints(sendpoints);
    
    return true;
}


bool ofxRayComposer::sendPoints(const vector<ofxLaser::Point>& points){
    
    vector<ofxIlda::PointDac> sendpoints(points.size());
    for(int i = 0; i<points.size(); i++) {
        ofxIlda::PointDac& p1 = sendpoints[i];
        ofxLaser::Point p2 = points[i];
        p1.x = ofMap(p2.x,0,800,kIldaMinPoint, kIldaMaxPoint);
        p1.y = ofMap(p2.y,800,0,kIldaMinPoint, kIldaMaxPoint); // Y is UP in ilda specs
        p1.r = p2.r/255.0f*kIldaMaxIntensity;
        p1.g = p2.g/255.0f*kIldaMaxIntensity;
        p1.b = p2.b/255.0f*kIldaMaxIntensity;
//        p1.i = 0;
        p1.u1 = 0;
        p1.u2 = 0;
    }
    addPoints(sendpoints);
    
    return true;
    
}

bool ofxRayComposer::setPointsPerSecond(uint32_t newpps){
    this->setPPS(newpps);
}

void ofxRayComposer::close(){
    // nothing to do
    // could use kill instead of deconstructor;
    // kill();
}


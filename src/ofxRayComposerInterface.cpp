//
//  Interface.cpp
//  ByensPuls
//
//  Created by Jonas Fehr on 04/08/2019.
//

#include "ofxRayComposerInterface.h"


//--------------------------------------------------------------

void ofxRayComposer::Interface::updateDeviceID(int index, bool bStartThread){
    int ret = RCDeviceID(index, deviceId, sizeof(deviceId));
    if(ret < 0){
        ofLogWarning() << "ofxRayComposer::Interface::init - Error reading device id! Exit.\n";
        //return -5;
    }
    state = RAYCOMPOSER_FOUND;

}

void ofxRayComposer::Interface::openDevice(){
    ofLogNotice() << "ofxRayComposer::Interface::init - Opening device: " <<  deviceId << endl;
    handle = RCOpenDevice(deviceId);
    if(handle < 0){
        ofLogWarning() << "ofxRayComposer::Interface::init - Error opening device: " << handle << "! Exit.\n";
        //return -6;
    }
    state = RAYCOMPOSER_OPENED;

}
void ofxRayComposer::Interface::startDevice(){
    ofLogNotice() << "ofxRayComposer::Interface::init - Starting laser." << endl;
    int ret = RCStartOutput(handle);
    if(ret < RCOk){
        ofLogWarning() << "ofxRayComposer::Interface::init - Error starting laser output: " << ret << "! Exit.\n";
        //return -7;
    }
    state = RAYCOMPOSER_STARTED;
}

//--------------------------------------------------------------
void ofxRayComposer::Interface::kill() {
    clear();
    if(state > RAYCOMPOSER_OPENED) {
//        printf("\nStoping laser.\n");
        int ret = RCStopOutput(handle);
        if(ret < RCOk){
            ofLogWarning() << "ofxRayComposer::Interface::kill - Error stoping laser output: " << ret << "! Exit.\n";
            //return -10;
        }
        state = RAYCOMPOSER_OPENED;
    }
    if(state > RAYCOMPOSER_FOUND) {
//        printf("Closing device.\n");
        int ret = RCCloseDevice(handle);
        if(ret < RCOk){
            ofLogWarning() << "ofxRayComposer::Interface::kill - Error closing device: " << ret << "! Exit.\n";
            //return -11;
        }
        state = RAYCOMPOSER_FOUND;
    }
    ofLogNotice() << "ofxRayComposer::Interface::kill - "<< deviceId <<" - done";
}

//--------------------------------------------------------------
void ofxRayComposer::Interface::threadedFunction() {
    while (isThreadRunning() != 0) {
        
        switch (state) {
            case RAYCOMPOSER_NOTFOUND:
                //DO SOMETHING IF NO HANDLER
                //if(bAutoConnect) init(connectionIndex, true);
                break;
                
            case RAYCOMPOSER_FOUND:
                openDevice();
                break;
                
            case RAYCOMPOSER_OPENED:
                startDevice();
                break;
                
            case RAYCOMPOSER_STARTED:
                    if(bExtraSafety){
                        if(checkLastUpdate()) send();
                        else sendBlack();
                    }
                    else send();
                break;
        }
    }
}

//--------------------------------------------------------------
void ofxRayComposer::Interface::send() {
    if(state < RAYCOMPOSER_STARTED || points.empty()) return;
    
//    int ret = RCWaitForReady(handle, -1);
     int ret = 10;
    if(bWaitForFreeBuffer) ret = RCWaitForReady(handle, -1);
    else{
        ret = RCWaitForReady(handle, 5);
        if(ret == 0) {
            points.clear();
            return;
        }
    }
    
    if(ret < RCOk){
        ofLogWarning() << "ofxRayComposer::Interface::send - Error waiting for free buffer: " << ret << " " << deviceId << "! Exit.\n";
        //return -8;
    }
    ret = RCWriteFrame(handle, (RCPoint*)points.data(), points.size(), pps, 0);
    
    if(ret < RCOk){
        ofLogWarning() << "ofxRayComposer::Interface::send - Error writing frame to device: " << ret << "! Exit.\n";
        //return -9;
    }
    points.clear();
}

//--------------------------------------------------------------
void ofxRayComposer::Interface::sendBlack() {
    points.clear();
    ofxIlda::PointDac p;
    p.x = 0;
    p.y = 0;
    p.r = p.g = p.b = p.u1 = p.u2 = 0;
    points.push_back(p);
    send();
}


//--------------------------------------------------------------
void ofxRayComposer::Interface::clear() {
        points.clear();
}

//--------------------------------------------------------------
void ofxRayComposer::Interface::addPoints(const vector<ofxIlda::PointDac>& _points) {
        if(!_points.empty()) {
            points.insert(points.end(), _points.begin(), _points.end());
        }
    updated();
}


//--------------------------------------------------------------
void ofxRayComposer::Interface::addPoints(const ofxIlda::Frame &ildaFrame) {
    addPoints(ildaFrame.getPointsDac());
}


//--------------------------------------------------------------
void ofxRayComposer::Interface::setPoints(const vector<ofxIlda::PointDac>& _points) {
        points = _points;
    updated();
}


//--------------------------------------------------------------
void ofxRayComposer::Interface::setPoints(const ofxIlda::Frame &ildaFrame) {
    setPoints(ildaFrame.getPointsDac());
}


//--------------------------------------------------------------
void ofxRayComposer::Interface::setPPS(int i) {
        pps = i;
}

//--------------------------------------------------------------
int ofxRayComposer::Interface::getPPS() const {
    return pps;
}

//--------------------------------------------------------------
void ofxRayComposer::Interface::updated(){
    lastUpdate = ofGetElapsedTimef();
}

//--------------------------------------------------------------
bool ofxRayComposer::Interface::checkLastUpdate(){
    return ofGetElapsedTimef()-lastUpdate < MAX_TIME_SAFE;
}

//--------------------------------------------------------------
// for compability with ofxLaser
//--------------------------------------------------------------
bool ofxRayComposer::Interface::sendFrame(const vector<ofxLaser::Point>& points){
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

//--------------------------------------------------------------
void ofxRayComposer::Interface::setWaitForFreeBuffer(bool b) {
        bWaitForFreeBuffer = b;
}

//--------------------------------------------------------------
bool ofxRayComposer::Interface::getWaitBeforeSend() const {
    return bWaitForFreeBuffer;
}



bool ofxRayComposer::Interface::sendPoints(const vector<ofxLaser::Point>& points){
    
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

bool ofxRayComposer::Interface::setPointsPerSecond(uint32_t newpps){
    this->setPPS(newpps);
	return true;
}

void ofxRayComposer::Interface::close(){
    // nothing to do
    // could use kill instead of deconstructor;
    // kill();
}


void ofxRayComposer::Interface::update(){
    switch (state) {
        case RAYCOMPOSER_NOTFOUND:
            //DO SOMETHING IF NO HANDLER
            //if(bAutoConnect) init(connectionIndex, true);
            break;
            
        case RAYCOMPOSER_FOUND:
            openDevice();
            break;
            
        case RAYCOMPOSER_OPENED:
            startDevice();
            break;
            
        case RAYCOMPOSER_STARTED:
            if(bExtraSafety){
                if(checkLastUpdate()) send();
                else sendBlack();
            }
            else send();
            break;
    }
}

//
//  ofxRayComposer.cpp
//  ofxIlda example basic
//
//  Created by Jonas Fehr on 05/04/16.
//
//

#include "ofxRayComposer.h"

//--------------------------------------------------------------
void ofxRayComposer::Handler::setup(bool bAutoUpdateList, int updateEveryXFrame) {

    /* Sleep for a bit over a second, to ensure that we see broadcasts
     * from all available DACs. */
    usleep(1000000);
    init();
    this->updateEveryXFrame = updateEveryXFrame;
    
    if(bAutoUpdateList) ofAddListener(ofEvents().update, this, &ofxRayComposer::Handler::updateList);
}

void ofxRayComposer::Handler::updateList(ofEventArgs& updateArgs){
    if(ofGetFrameNum() % (updateEveryXFrame) == 0){ // aprox each 10 sec.
        init();
    }
}

//--------------------------------------------------------------
void ofxRayComposer::Handler::init() {
    /* Initialise the Library. API Version is returned. */
    int ret = RCInit();
    if(ret < 0){
        ofLogWarning() << "ofxRayComposer::Handler::init - Error initialising Library! Exit.\n";
        return -1;
    }
    if(ret < RCAPI_VERSION){
        ofLogWarning() << "ofxRayComposer::Handler::init - API Version too old. Please use a newer dll/lib.\n";
        return -2;
    }
    ofLogNotice() << "ofxRayComposer::Handler::init - API Version " << (ret >> 8) << "."<< (ret & 0xFF) << "\n"; //printf("API Version %d.%02d\n", ret >> 8, ret & 0xFF);
    
    /* Detect & enumerate the connected devices. */
    count = RCEnumerateDevices();
    if(count < 0){
        ofLogWarning() << "ofxRayComposer::Handler::init - Error enumerating devices! Exit.\n";
        return -3;
    }
    if(count == 0){
        ofLogWarning() << "ofxRayComposer::Handler::init - No devices found. Exit.\n";
        return 0;
    }
    
    /* List the devices found. */
    ofLogNotice() << "ofxRayComposer::Handler::init - Found " << count << " device(s).";

    for(int i = 0; i < count; i++){
        auto interface = make_shared<Interface>(i);
        string newDeviceID = interface->getDeviceID();
        if (!interfaces.count(newDeviceID)>0){
            //interface->start();
            ofLogNotice() << "ofxRayComposer::Handler::init - new Interface Found : " << interface->getConnectionIndex() << " - " << interface->getDeviceID() << endl;
            interfaces[newDeviceID] = interface;
            ofNotifyEvent(newDeviceFoundE, newDeviceID, this);
        }
    }

}

//--------------------------------------------------------------
void ofxRayComposer::Handler::kill() {
    for(auto & interface : interfaces){
        interface.second->kill();
    }
    int ret = RCExit();
    if(ret < RCOk){
        ofLogWarning() << "ofxRayComposer::Handler::kill - Error closing Library: " << ret << "! Exit.\n";
        //return -12;
    }
    ofLogNotice() << "ofxRayComposer::Handler::kill - done";

}

//--------------------------------------------------------------

std::shared_ptr<ofxRayComposer::Interface> ofxRayComposer::Handler::getInterface(string deviceId){
    if (interfaces.count(deviceId)>0){
        ofLogNotice() << "ofxRayComposer::Handler::getInterface - DeviceID: " << deviceId << " - found." << endl;
        return interfaces[deviceId];
    } else {
        ofLogWarning() << "ofxRayComposer::Handler::getInterface - DeviceID: " << deviceId << " - not found.\n";
        return NULL;
    }
}

bool ofxRayComposer::Handler::referenceInterface(std::shared_ptr<Interface> & interface, string deviceId){
    std::shared_ptr<Interface> foundInterface = getInterface(deviceId);
    if (foundInterface != NULL){
        interface = foundInterface;
        ofLogNotice() << "ofxRayComposer::Handler::referenceInterface - successfull.\n";
        return true;
    } else {
        ofLogWarning() << "ofxRayComposer::Handler::referenceInterface - device not found.\n";
        
//        if(interfaces.size()>0){
//            for(auto & interface : interfaces){
//                if(interface.second->getState() == 1) deviceId = interface.second->getDeviceID();
//                break;
//            }
//            referenceInterface(interface, deviceId);
//        }
        return false;
    }
}


vector<string> ofxRayComposer::Handler::getListOfDeviceIDs(){
    vector<string> deviceIDs;
    for(auto & interface : interfaces){
        deviceIDs.push_back(interface.second->getDeviceID());
    }
    return deviceIDs;
}

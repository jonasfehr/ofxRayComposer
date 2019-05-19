//
//  ofxRayComposer.hpp
//  ofxIlda example basic
//
//  Created by Jonas Fehr on 05/04/16.
//
//


#include <stdio.h>
#include "rcdev.h"
#include "ofMain.h"
#include "ofxIldaFrame.h"

#include "ofxLaserClasses.h"

//#include "ofxLaserPoint.h"
//#include "ofxLaserDacBase.h"

#define MAX_TIME_SAFE 0.25 //sec

class ofxRayComposer : public ofThread, public ofxLaser::DacBase {
public:
    ofxRayComposer():state(RAYCOMPOSER_NOTFOUND), bAutoConnect(false) {}
    
    ~ofxRayComposer() {
        kill();
    }
    
    bool stateIsFound();
    void setup(bool bStartThread = true, int idRayComposer = 0, bool extraSafety = true);

    virtual void threadedFunction();
    
    void clear();
    void start();
    void stop();
    
    void addPoints(const vector<ofxIlda::PointDac>& _points);
    void addPoints(const ofxIlda::Frame &ildaFrame);
    
    void setPoints(const vector<ofxIlda::PointDac>& _points);
    void setPoints(const ofxIlda::Frame &ildaFrame);
    
    void send();
    void sendBlack();
    
    void setPPS(int i);
    int getPPS() const;
    
    void setWaitBeforeSend(bool b);
    bool getWaitBeforeSend() const;
    
    string getDeviceID(){return deviceId;};
    int getConnectionID(){return idRayComposerConnection;};

    // Compatibility to ofxLaser : https://github.com/sebleedelisle/ofxLaser
    void setup(string ip){this->setup();};
    bool sendFrame(const vector<ofxLaser::Point>& points);
    bool sendPoints(const vector<ofxLaser::Point>& points);
    bool setPointsPerSecond(uint32_t newpps);
    void close();

    
private:
    void init();
    void kill();
    
private:
    enum {
        RAYCOMPOSER_NOTFOUND = 0,
        RAYCOMPOSER_FOUND
    } state;
    
    int pps;
    bool bWaitBeforeSend;
    bool bAutoConnect;
    bool bExtraSafety;
    
    //struct etherdream *device;
    vector<ofxIlda::PointDac> points;
    
    int idRayComposerConnection;
    
    int count; /* device count */
    char deviceId[256]; /* device id string */
    int handle; /* device handle */
    
    float lastUpdate = 0;
    void updated();
    bool checkLastUpdate();
};


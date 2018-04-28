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


class ofxRayComposer : public ofThread, public ofxLaser::DacBase {
public:
    ofxRayComposer():state(RAYCOMPOSER_NOTFOUND), bAutoConnect(false) {}
    
    ~ofxRayComposer() {
        kill();
    }
    
    bool stateIsFound();
    void setup(bool bStartThread = true, int idRayComposer = 0);
    virtual void threadedFunction();
    
    void clear();
    void start();
    void stop();
    
    void addPoints(const vector<ofxIlda::Point>& _points);
    void addPoints(const ofxIlda::Frame &ildaFrame);
    
    void setPoints(const vector<ofxIlda::Point>& _points);
    void setPoints(const ofxIlda::Frame &ildaFrame);
    
    void send();
    
    void setPPS(int i);
    int getPPS() const;
    
    void setWaitBeforeSend(bool b);
    bool getWaitBeforeSend() const;
    
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
    
    //struct etherdream *device;
    vector<ofxIlda::Point> points;
    
    int idRayComposerConnection;
    
    int count; /* device count */
    char deviceId[256]; /* device id string */
    int handle; /* device handle */
};


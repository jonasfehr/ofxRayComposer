//
//  Interface.hpp
//  ByensPuls
//
//  Created by Jonas Fehr on 04/08/2019.
//

#ifndef Interface_hpp
#define Interface_hpp

#include <stdio.h>
#include "rcdev.h"
#include "ofMain.h"
#include "ofxIldaFrame.h"
#include "ofxLaserClasses.h"

#define MAX_TIME_SAFE 0.25 //sec

namespace ofxRayComposer {
    
    class Interface : public ofThread, public ofxLaser::DacBase {
    public:
        Interface(int connectionIndex, bool bStartThread = false, bool extraSafety = true) : state(RAYCOMPOSER_NOTFOUND), bAutoConnect(false){
            updateDeviceID(connectionIndex, bStartThread);
            
            bExtraSafety = extraSafety;
            
            this->connectionIndex = connectionIndex;
            
            setPPS(30000);
            setWaitBeforeSend(false);
        }
        
        ~Interface() {

        }
        
        
        virtual void threadedFunction();
        
        void updateDeviceID(int index, bool bStartThread = false);
        void openDevice();
        void startDevice();

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
        int getConnectionIndex(){return connectionIndex;};
        
        // Compatibility to ofxLaser : https://github.com/sebleedelisle/ofxLaser
        void setup(string ip){this->start();};
        bool sendFrame(const vector<ofxLaser::Point>& points);
        bool sendPoints(const vector<ofxLaser::Point>& points);
        bool setPointsPerSecond(uint32_t newpps);
        void close();
        
        
        void kill();
        
    private:
        enum {
            RAYCOMPOSER_NOTFOUND = 0,
            RAYCOMPOSER_FOUND,
            RAYCOMPOSER_OPENED,
            RAYCOMPOSER_STARTED
        } state;
        
        int pps;
        bool bWaitBeforeSend;
        bool bAutoConnect;
        bool bExtraSafety;
        
        //struct etherdream *device;
        vector<ofxIlda::PointDac> points;
        
        char deviceId[256]; /* device id string */
        int handle; /* device handle */
        int connectionIndex;
        
        float lastUpdate = 0;
        void updated();
        bool checkLastUpdate();
    };
    
}


#endif /* Interface_hpp */


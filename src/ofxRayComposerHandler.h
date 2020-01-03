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

#include "ofxRayComposerInterface.h"

namespace ofxRayComposer{
    class Handler{
    public:
        Handler(){

        }
        
        ~Handler() {
            kill();
        }
        
        void setup(bool autoUpdateList = true, int updateEveryXFrame = 60*5);
        
        std::shared_ptr<Interface> getInterface(string deviceId);
        bool referenceInterface(std::shared_ptr<Interface> & interface, string deviceId);
        
        vector<string> getListOfDeviceIDs();
        void kill();

    private:
        void init();
        void updateList(ofEventArgs& updateArgs);
        int updateEveryXFrame;
    public:
        int count; /* device count */
        map<string, std::shared_ptr<Interface>> interfaces;
        
        ofEvent<string> newDeviceFoundE;

    };
}



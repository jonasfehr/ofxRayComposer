#pragma once

#include "ofMain.h"
#include "ofxRayComposer.h"
#include "ofxIldaFrame.h"

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed  (int key);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    
    
    
    ofxIlda::Frame ildaFrame;   // stores and manages ILDA frame drawings

    ofxRayComposer rayComposer;
};

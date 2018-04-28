//
//  ofxLaserClasses.h
//  Laser_tools_V2
//
//  Created by Jonas Fehr on 28/04/2018.
//  for compatibility with ofxLaser
//  https://github.com/sebleedelisle/ofxLaser


#ifndef ofxLaserClasses_h
#define ofxLaserClasses_h

class ofxLaserPoint : public ofPoint {
    
public:
    
    ofxLaserPoint() {
        x = y = z = 0;
        r = g = b = 0;
        useCalibration = true;
        intensity = 1;
        
    }
    ofxLaserPoint(ofPoint p, ofColor c, float pointIntensity = 1, bool usecalibration = true ) {
        
        x = p.x;
        y = p.y;
        z = p.z;
        r = c.r;
        g = c.g;
        b = c.b;
        useCalibration = usecalibration;
        intensity = pointIntensity;
    }
    
    ofColor getColor() {
        return ofColor(r, g, b);
    }
    
    float r; //0-255
    float g;
    float b;
    
    float intensity;
    bool useCalibration;
    
    
};

class DacBase {
public:
    DacBase() {};
    
    virtual bool sendFrame(const vector<Point>& points) { return true; };
    virtual bool sendPoints(const vector<Point>& points) { return true; };
    virtual bool setPointsPerSecond(uint32_t pps) { return true; };
    
};


#endif /* ofxLaserClasses_h */

//
//  ofxShadowSimple.h
//  emptyExample
//
//  Created by Nick Hardeman on 8/4/15.
//

#pragma once
#include "ofMain.h"

#define STRINGIFY(x) #x

class ofxShadowSimple {
public:
    
    ofxShadowSimple();
    
    void load( string aVertPath, string aFragPath );
    void setRange( float nearClip, float farClip );
    void setLightPosition( ofVec3f aPos );
    void setLightLookAt( ofVec3f aPos, ofVec3f upVector = ofVec3f(0, 1, 0) );
    
    void beginDepthPass();
    void endDepthPass();
    
    void beginRenderPass( ofCamera& aCam );
    void endRenderPass();
    
    void setWidth( float aWidth );
    void setHeight( float aHeight );
    float getWidth();
    float getHeight();
    
    // bias to reduce shadow acne //
    void setBias( float aBias );
    // intensity of the shadows //
    void setIntensity( float aIntensity );
    
protected:
    
    string fragShaderStr, vertShaderStr;
    
    ofMatrix4x4 biasMatrix;
    
    void allocateFbo();
    
    bool bTriedLoad;
    
    float _width, _height;
    float _depthBias;
    float _intensity;
    
    ofFbo shadowFbo;
    ofCamera lightCam;
    ofShader shader;
};

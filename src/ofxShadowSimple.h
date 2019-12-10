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
    
    void beginDepthPass( bool aBWithCam = true );
    void endDepthPass( bool aBWithCam = true );
    
    void beginRenderPass( ofCamera& aCam );
    void endRenderPass();
    void setShaderData( ofShader* ashader, ofCamera& aCam, int atexLoc=2 );
    
    void setWidth( float aWidth );
    void setHeight( float aHeight );
    float getWidth();
    float getHeight();
    
    // bias to reduce shadow acne //
    void setBias( float aBias );
    // intensity of the shadows //
    void setIntensity( float aIntensity );
    
    ofFbo& getFbo() { return shadowFbo; }
    ofCamera& getLightCamera() { return lightCam; }
    ofMatrix4x4 getShadowTransMatrix( ofCamera& acam );
    ofShader& getShader() { return shader; }
    ofParameterGroup& getParams() { return mParams; }
    
protected:
    
    string fragShaderStr, vertShaderStr;
    
    ofMatrix4x4 biasMatrix;
    
    void allocateFbo();
    
    bool bTriedLoad = false;
    
    ofParameterGroup mParams;
    ofParameter<float> _width, _height, _depthBias, _intensity;
    ofParameter<float> _nearClip, _farClip;
    
    ofFbo shadowFbo;
    ofCamera lightCam;
    ofShader shader;
};

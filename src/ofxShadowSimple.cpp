//
//  ofxShadowSimple.cpp
//  emptyExample
//
//  Created by Nick Hardeman on 8/4/15.
//

#include "ofxShadowSimple.h"

#define GLSL410(shader)  "#version 410 \n" #shader

//--------------------------------------------------------------
ofxShadowSimple::ofxShadowSimple() {
//    _width, _height, _depthBias, _intensity;
    _width.set( "u_shadowWidth", ofGetWidth());
    _height.set("u_shadowHeight", ofGetHeight() );
    _depthBias.set("u_shadowBias", 0.001, 0.00001, 0.01 );
    _intensity.set("u_shadowIntensity", 0.7, 0.0, 1.0 );
    _nearClip.set("ShadowNearClip", 1, 0, 1000 );
    _farClip.set("ShadowFarClip", 1000, 0, 5000 );
    
    mParams.setName("ofxShadowSimple");
    mParams.add( _depthBias );
    mParams.add( _intensity );
    mParams.add( _nearClip );
    mParams.add( _farClip );
    
//    setWidth( ofGetWidth() );
//    setHeight( ofGetHeight() );
//    setRange( _nearClip, _farClip );
    
    biasMatrix = ofMatrix4x4(
                           0.5, 0.0, 0.0, 0.0,
                           0.0, 0.5, 0.0, 0.0,
                           0.0, 0.0, 0.5, 0.0,
                           0.5, 0.5, 0.5, 1.0
                           );
    
//    setBias( 0.001 );
//    setIntensity( 0.7 );
    
    bTriedLoad = false;
    
    vertShaderStr = GLSL410(
                            uniform mat4 modelMatrix;
                              uniform mat4 modelViewProjectionMatrix;
                              uniform mat4 modelViewMatrix;
                              
                              uniform mat4 u_shadowTransMatrix;
                              
                              in vec4 position;
                            in vec4 color;
                              
                              out vec4 oVertInLightSpace;
                              
                              void main( void ) {
                                    vec4 vertInViewSpace = modelViewMatrix * position;
                                    oVertInLightSpace = u_shadowTransMatrix * vertInViewSpace;
                                    gl_Position = modelViewProjectionMatrix * position;
                              }
    );
    
    
    fragShaderStr = GLSL410(
                              uniform sampler2D   u_shadowMap;

                              // make sure to send this from the vert to the frag //
                              in vec4 oVertInLightSpace;

                              uniform float       u_shadowBias;
                              uniform float       u_shadowWidth;
                              uniform float       u_shadowHeight;

                              uniform float       u_shadowIntensity;

//                              uniform mat4        u_shadowTransMatrix;

                              // float a[5] = float[](3.4, 4.2, 5.0, 5.2, 1.1);
                              vec2 poissonDisk[16] = vec2[](
                                  vec2( -0.94201624, -0.39906216 ),
                                  vec2( 0.94558609, -0.76890725 ),
                                  vec2( -0.094184101, -0.92938870 ),
                                  vec2( 0.34495938, 0.29387760 ),
                                  vec2( -0.91588581, 0.45771432 ),
                                  vec2( -0.81544232, -0.87912464 ),
                                  vec2( -0.38277543, 0.27676845 ),
                                  vec2( 0.97484398, 0.75648379 ),
                                  vec2( 0.44323325, -0.97511554 ),
                                  vec2( 0.53742981, -0.47373420 ),
                                  vec2( -0.26496911, -0.41893023 ),
                                  vec2( 0.79197514, 0.19090188 ),
                                  vec2( -0.24188840, 0.99706507 ),
                                  vec2( -0.81409955, 0.91437590 ),
                                  vec2( 0.19984126, 0.78641367 ),
                                  vec2( 0.14383161, -0.14100790 )
                                                            );

                              float getShadow( vec4 aVertInLightSpace ) {
                                  
                                  // get projected shadow value
                                  vec3 tdepth = aVertInLightSpace.xyz / aVertInLightSpace.w;
                                  vec4 depth  = vec4( tdepth.xyz, aVertInLightSpace.w );
                                  
                                  depth.y = 1.0 - depth.y;
                                  
                                  float shadow = 1.0;
                                  vec2 UVCoords = depth.xy;
                                  
                              //    float stexel = texture2D( u_ShadowMap, depth.xy ).r;
                                  int numSamples = 16;
                                  float shadowDec = 1.0/float(numSamples);
                                  for( int i = 0; i < numSamples; i++ ) {
                                      float texel = texture( u_shadowMap, UVCoords.xy + (poissonDisk[i]/(u_shadowWidth*0.75)) ).r;
                                      if( texel < depth.z - u_shadowBias ) {
                                          shadow -= shadowDec * u_shadowIntensity;
                                      }
                                  }
                                  shadow = clamp( shadow, 0.0, 1.0 );
                                  
                                  // are you behind the shadow view? //
                                  if( aVertInLightSpace.z < 1.0) {
                                      shadow = 1.0;
                                  }
                                  
                                  return shadow;
                              }
                              
                              uniform vec4 globalColor;
                              out vec4 outColor;
                              
                              void main() {
                                outColor = globalColor;
                                  float shade = getShadow( oVertInLightSpace );
        outColor.rgb = shade * globalColor.rgb;
                              }
                              
                    );
}

//--------------------------------------------------------------
void ofxShadowSimple::load( string aVertPath, string aFragPath ) {
    bTriedLoad = true;
    shader.load( aVertPath, aFragPath );
}

//--------------------------------------------------------------
void ofxShadowSimple::setRange( float nearClip, float farClip ) {
    lightCam.setNearClip( nearClip );
    lightCam.setFarClip( farClip );
    _nearClip = nearClip;
    _farClip = farClip;
}

//--------------------------------------------------------------
void ofxShadowSimple::setLightPosition( ofVec3f aPos ) {
    lightCam.setPosition( aPos );
}

//--------------------------------------------------------------
void ofxShadowSimple::setLightLookAt( ofVec3f aPos, ofVec3f upVector ) {
    lightCam.lookAt( aPos, upVector );
}

//--------------------------------------------------------------
void ofxShadowSimple::beginDepthPass( bool aBWithCam ) {
    
    if( lightCam.getNearClip() != _nearClip || lightCam.getFarClip() != _farClip ) {
        setRange(_nearClip, _farClip );
    }
    
    if( !shadowFbo.isAllocated() || shadowFbo.getWidth() != getWidth() || shadowFbo.getHeight() != getHeight() ) {
        allocateFbo();
    }
    
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    
    shadowFbo.begin();
    ofClear(255);
    if(aBWithCam) lightCam.begin();
    
    
//    glEnable( GL_CULL_FACE ); // cull front faces - this helps with artifacts and shadows with exponential shadow mapping
//    glCullFace( GL_BACK );
    
    
}

//--------------------------------------------------------------
void ofxShadowSimple::endDepthPass(bool aBWithCam) {
    if(aBWithCam) lightCam.end();
    shadowFbo.end();
    
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    
//    glCullFace( GL_BACK );
//    glDisable( GL_CULL_FACE );
}

//--------------------------------------------------------------
void ofxShadowSimple::beginRenderPass( ofCamera &aCam ) {
    
    if( !bTriedLoad ) {
        shader.setupShaderFromSource( GL_VERTEX_SHADER, vertShaderStr );
        shader.setupShaderFromSource( GL_FRAGMENT_SHADER, fragShaderStr );
        shader.linkProgram();
        bTriedLoad = true;
    }
    
    shader.begin();
    setShaderData( &shader, aCam, 3 );
    
//    ofMatrix4x4 inverseCameraMatrix = ofMatrix4x4::getInverseOf( aCam.getModelViewMatrix() );
//    ofMatrix4x4 shadowTransMatrix = inverseCameraMatrix * lightCam.getModelViewMatrix() * lightCam.getProjectionMatrix() * biasMatrix;
//
//    shader.begin();
//    shader.setUniformTexture( "u_shadowMap", shadowFbo.getDepthTexture(), 3 );
////    glm::vec3 camPosInViewSpace = glm::vec3(aCam.getPosition() * aCam.getModelViewMatrix());
//    shader.setUniformMatrix4f("u_shadowTransMatrix", shadowTransMatrix );
////    ofVec3f lpos = glm::vec4(lightCam.getPosition(), 1.0) * aCam.getModelViewMatrix();
////    shader.setUniform3fv( "u_lightPosInWorldSpace", &lpos.getPtr()[0] );
////    ofVec3f lightInViewSpace = lightCam.getPosition() * aCam.getModelViewMatrix();
//
//    shader.setUniform1f(_width.getName(), getWidth() );
//    shader.setUniform1f( _height.getName(), getHeight() );
//    shader.setUniform1f(_depthBias.getName(), _depthBias );
//    shader.setUniform1f(_intensity.getName(), _intensity );
}

//--------------------------------------------------------------
void ofxShadowSimple::endRenderPass() {
    shader.end();
}

//--------------------------------------------------------------
void ofxShadowSimple::setShaderData( ofShader* ashader, ofCamera& aCam, int atexLoc ) {
    ashader->setUniformTexture( "u_shadowMap", shadowFbo.getDepthTexture(), atexLoc );
    ofMatrix4x4 inverseCameraMatrix = ofMatrix4x4::getInverseOf( aCam.getModelViewMatrix() );
    ofMatrix4x4 shadowTransMatrix = inverseCameraMatrix * lightCam.getModelViewMatrix() * lightCam.getProjectionMatrix() * biasMatrix;
    ashader->setUniformMatrix4f("u_shadowTransMatrix", shadowTransMatrix );
    
    ashader->setUniform1f(_width.getName(), getWidth() );
    ashader->setUniform1f(_height.getName(), getHeight() );
    ashader->setUniform1f(_depthBias.getName(), _depthBias );
    ashader->setUniform1f(_intensity.getName(), _intensity );
    ashader->setUniform3f("u_shadowLightPos", getLightCamera().getPosition() );
}

//--------------------------------------------------------------
void ofxShadowSimple::allocateFbo() {
    ofFbo::Settings settings;
    settings.width  = getWidth();
    settings.height = getHeight();
    settings.textureTarget = GL_TEXTURE_2D;
    settings.internalformat = GL_RGBA32F_ARB;
    settings.useDepth = true;
    settings.depthStencilAsTexture = true;
    settings.useStencil = true;
//    settings.depthStencilInternalFormat = GL_DEPTH_COMPONENT32;
    
    shadowFbo.allocate( settings );
}

//--------------------------------------------------------------
void ofxShadowSimple::setWidth( float aWidth ) {
    int tw = aWidth;
    _width = tw;
}

//--------------------------------------------------------------
void ofxShadowSimple::setHeight( float aHeight ) {
    int th = aHeight;
    _height = th;
}

//--------------------------------------------------------------
float ofxShadowSimple::getWidth() {
    return _width;
}

//--------------------------------------------------------------
float ofxShadowSimple::getHeight() {
    return _height;
}

//--------------------------------------------------------------
void ofxShadowSimple::setBias( float aBias ) {
    _depthBias = aBias;
}

//--------------------------------------------------------------
void ofxShadowSimple::setIntensity( float aIntensity ) {
    _intensity = aIntensity;
}

//--------------------------------------------------------------
ofMatrix4x4 ofxShadowSimple::getShadowTransMatrix( ofCamera& acam ) {
    ofMatrix4x4 inverseCameraMatrix = ofMatrix4x4::getInverseOf( acam.getModelViewMatrix() );
    ofMatrix4x4 shadowTransMatrix = inverseCameraMatrix * lightCam.getModelViewMatrix() * lightCam.getProjectionMatrix() * biasMatrix;
    return shadowTransMatrix;
}






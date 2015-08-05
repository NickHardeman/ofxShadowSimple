//
//  ofxShadowSimple.cpp
//  emptyExample
//
//  Created by Nick Hardeman on 8/4/15.
//

#include "ofxShadowSimple.h"

//--------------------------------------------------------------
ofxShadowSimple::ofxShadowSimple() {
    setWidth( ofGetWidth() );
    setHeight( ofGetHeight() );
    setRange( 1, 1000 );
    
    biasMatrix = ofMatrix4x4(
                           0.5, 0.0, 0.0, 0.0,
                           0.0, 0.5, 0.0, 0.0,
                           0.0, 0.0, 0.5, 0.0,
                           0.5, 0.5, 0.5, 1.0
                           );
    
    setBias( 0.001 );
    setIntensity( 0.7 );
    
    bTriedLoad = false;
    
    vertShaderStr = STRINGIFY( 
                              varying vec3		v_Normal;
                              varying vec4		v_VertInLightSpace;
                              varying vec3        v_LightDir;
                              
                              uniform mat4		u_ShadowTransMatrix;
                              uniform vec3        u_lightPosInWorldSpace;
                              
                              void main( void ) {
                                  vec4 vertInViewSpace = gl_ModelViewMatrix * gl_Vertex;
                                  
                                  v_Normal = gl_NormalMatrix * gl_Normal;
                                  v_LightDir = ( u_lightPosInWorldSpace - vertInViewSpace.xyz );
                                  
                                  v_VertInLightSpace = u_ShadowTransMatrix * vertInViewSpace;
                                  
                                  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
                                  
                                  gl_FrontColor = gl_Color;
                              }
    );
    
    
    fragShaderStr = STRINGIFY( uniform sampler2D   tShadowMap;
                                   
                                   varying vec3		v_Normal;
                                   varying vec4		v_VertInLightSpace;
                                   varying vec3        v_LightDir;
                                   
                                   uniform float       u_bias;
                                   uniform float       u_width;
                                   uniform float       u_height;
                                   
                                   uniform float       u_shadowIntensity;
                                   
                                   vec2 poissonDisk[16];
                                   
                                   void main( void ) {
                                       
//                                       poissonDisk[0] = vec2( -0.94201624, -0.39906216 );
//                                       poissonDisk[1] = vec2( 0.94558609, -0.76890725 );
//                                       poissonDisk[2] = vec2( -0.094184101, -0.92938870 );
//                                       poissonDisk[3] = vec2( 0.34495938, 0.29387760 );
                                       
                                       poissonDisk[0] = vec2( -0.94201624, -0.39906216 );
                                       poissonDisk[1] = vec2( 0.94558609, -0.76890725 );
                                       poissonDisk[2] = vec2( -0.094184101, -0.92938870 );
                                       poissonDisk[3] = vec2( 0.34495938, 0.29387760 );
                                       poissonDisk[4] = vec2( -0.91588581, 0.45771432 );
                                       poissonDisk[5] = vec2( -0.81544232, -0.87912464 );
                                       poissonDisk[6] = vec2( -0.38277543, 0.27676845 );
                                       poissonDisk[7] = vec2( 0.97484398, 0.75648379 );
                                       poissonDisk[8] = vec2( 0.44323325, -0.97511554 );
                                       poissonDisk[9] = vec2( 0.53742981, -0.47373420 );
                                       poissonDisk[10] = vec2( -0.26496911, -0.41893023 );
                                       poissonDisk[11] = vec2( 0.79197514, 0.19090188 );
                                       poissonDisk[12] = vec2( -0.24188840, 0.99706507 );
                                       poissonDisk[13] = vec2( -0.81409955, 0.91437590 );
                                       poissonDisk[14] = vec2( 0.19984126, 0.78641367 );
                                       poissonDisk[15] = vec2( 0.14383161, -0.14100790 );
                                       
                                       vec3 normal     = normalize(v_Normal);
                                       vec3 lightDir   = v_LightDir;
                                       
                                       float lambert = max(dot(normal, normalize(lightDir) ), 0.0);
                                       
                                       // get projected shadow value
                                       vec3 tdepth = v_VertInLightSpace.xyz / v_VertInLightSpace.w;
                                       vec4 depth  = vec4( tdepth.xyz, v_VertInLightSpace.w );
                                       
                                       depth.y = 1.0 - depth.y;
                                       
                                       float shadow = 1.0;
                                       vec2 UVCoords = depth.xy;
                                       
                                       float stexel = texture2D( tShadowMap, depth.xy ).r;
                                       int numSamples = 16;
                                       float shadowDec = 1.0/float(numSamples);
                                       for( int i = 0; i < numSamples; i++ ) {
                                           float texel = texture2D( tShadowMap, UVCoords.xy + (poissonDisk[i]/(u_width*0.75)) ).r;
                                           if( texel < depth.z - u_bias ) {
                                               shadow -= shadowDec * u_shadowIntensity;
                                           }
                                       }
                                       shadow = clamp( shadow, 0.0, 1.0 );
                                       
                                       // are you behind the shadow view? //
                                       if( v_VertInLightSpace.z < 1.0) {
                                           shadow = 1.0;
                                       }
                                       
                                       gl_FragColor.a = 1.0;
                                       
                                       vec4 diffuse = gl_Color * clamp( lambert, 0.0, 1.0 ) * shadow;
                                       
                                       gl_FragColor.xyz = diffuse.xyz;
                                       
                                   } );
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
void ofxShadowSimple::beginDepthPass() {
    if( !bTriedLoad ) {
        shader.setupShaderFromSource( GL_VERTEX_SHADER, vertShaderStr );
        shader.setupShaderFromSource( GL_FRAGMENT_SHADER, fragShaderStr );
        shader.linkProgram();
        bTriedLoad = true;
    }
    
    if( !shadowFbo.isAllocated() || shadowFbo.getWidth() != getWidth() || shadowFbo.getHeight() != getHeight() ) {
        allocateFbo();
    }
    
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    
    shadowFbo.begin();
    ofClear(255);
    lightCam.begin();
    
    
//    glEnable( GL_CULL_FACE ); // cull front faces - this helps with artifacts and shadows with exponential shadow mapping
//    glCullFace( GL_BACK );
    
    
}

//--------------------------------------------------------------
void ofxShadowSimple::endDepthPass() {
    lightCam.end();
    shadowFbo.end();
    
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    
//    glCullFace( GL_BACK );
//    glDisable( GL_CULL_FACE );
}

//--------------------------------------------------------------
void ofxShadowSimple::beginRenderPass( ofCamera &aCam ) {
    
    ofMatrix4x4 inverseCameraMatrix = ofMatrix4x4::getInverseOf( aCam.getModelViewMatrix() );
    ofMatrix4x4 shadowTransMatrix = inverseCameraMatrix * lightCam.getModelViewMatrix() * lightCam.getProjectionMatrix() * biasMatrix;
    
    shader.begin();
    shader.setUniformTexture( "tShadowMap", shadowFbo.getDepthTexture(), 3 );
    ofVec3f camPosInViewSpace = aCam.getPosition() * aCam.getModelViewMatrix();
    shader.setUniformMatrix4f("u_ShadowTransMatrix", shadowTransMatrix );
    ofVec3f lpos = lightCam.getPosition() * aCam.getModelViewMatrix();
    shader.setUniform3fv( "u_lightPosInWorldSpace", &lpos.getPtr()[0] );
    ofVec3f lightInViewSpace = lightCam.getPosition() * aCam.getModelViewMatrix();
    
    shader.setUniform1f( "u_width", getWidth() );
    shader.setUniform1f( "u_height", getHeight() );
    shader.setUniform1f("u_bias", _depthBias );
    shader.setUniform1f("u_shadowIntensity", _intensity );
}

//--------------------------------------------------------------
void ofxShadowSimple::endRenderPass() {
    shader.end();
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
    _width = aWidth;
}

//--------------------------------------------------------------
void ofxShadowSimple::setHeight( float aHeight ) {
    _height = aHeight;
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






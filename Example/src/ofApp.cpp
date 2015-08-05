#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    
    // make sure to copy the shaders folder from addons/ofxShadowsSimple/shaders to the bin folder //
    
    ofSetBoxResolution( 30, 30, 30 );
    
    cam.disableMouseInput();
    cam.setDistance( 10 );
    cam.setPosition(0, 0, -10 );
    cam.lookAt( ofVec3f(0,0,0), ofVec3f(0,-1,0) );
    cam.setNearClip(1);
    cam.setFarClip(250);
    
    cam.enableMouseInput();
    
    shadow.setRange( 10, 250 );
    shadow.setBias( 0.01 );
    
    bunny.load( "lofi-bunny.ply" );
    
    vector< ofMeshFace > faces = bunny.getUniqueFaces();
    for( int i = 0; i < faces.size(); i++ ) {
        faces[i].setVertex( 0, faces[i].getVertex(0 ));
        faces[i].setNormal(0, faces[i].getFaceNormal() );
        faces[i].setNormal(1, faces[i].getFaceNormal() );
        faces[i].setNormal(2, faces[i].getFaceNormal() );
    }
    bunny.setFromTriangles( faces );
    bunny.smoothNormals( 60 );
    cout << "Bunny normals = " << bunny.getNumNormals() << endl;
}

//--------------------------------------------------------------
void ofApp::update(){
    
    shadow.setLightPosition( ofVec3f(cos(ofGetElapsedTimef()*0.6) * 10, -3, -25) );
    shadow.setLightLookAt( ofVec3f(0,0,0), ofVec3f(0,-1,0) );
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    shadow.beginDepthPass();
    glEnable(GL_DEPTH_TEST);
    renderScene();
    glDisable(GL_DEPTH_TEST);
    shadow.endDepthPass();
    
    
    shadow.beginRenderPass( cam );
    cam.begin();
    glEnable(GL_DEPTH_TEST);
    renderScene();
    glDisable(GL_DEPTH_TEST);
    cam.end();
    shadow.endRenderPass();
    
    
}

//--------------------------------------------------------------
void ofApp::renderScene() {
    
    ofBackground( 241,212,55 );
    
    ofSetColor( 241,238,162 );
    ofDrawBox( 2, 2, 2 );
    
    ofSetColor( 241,221,113 );
    ofDrawSphere( -4, sin( ofGetElapsedTimef() ) * 3, 2 );
    ofSetColor( 183,241,195 );
    ofDrawSphere( -4, sin( ofGetElapsedTimef() * 0.1 ) * 3, 5, 2 );
    
    
    ofSetColor( 241,212,55 );
    ofPushMatrix(); {
        ofTranslate(5, 0, 0 );
        ofRotate(180, 1, 0, 0 );
        ofScale( 0.015, 0.015, 0.015 );
        bunny.draw();
    } ofPopMatrix();
    
    // floor //
    ofSetColor( 142,187,151 );
    ofDrawBox( 0, 5, 0, 250, 2, 250 );
    
    ofDrawBox(0, -8, 10, 80, 30, 2 );
    
    
    
//    ofDrawSphere( light.getPosition(), 1 );
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
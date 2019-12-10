#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    
    
//	ofSetupOpenGL(1024,768, OF_WINDOW);			// <-------- setup the GL context
    ofGLFWWindowSettings settings;
    settings.setSize(1200, 800);
    settings.setGLVersion(3, 2);
    auto mainWindow = ofCreateWindow( settings );

	auto app = make_shared<ofApp>();
    ofRunApp( mainWindow, app );
    ofRunMainLoop();

}

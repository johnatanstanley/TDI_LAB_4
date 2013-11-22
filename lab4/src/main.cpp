#include "ofMain.h"
#include "testApp.h"

//========================================================================
int main( ){
	ofSetupOpenGL(1300,280, OF_WINDOW);
	ofRunApp(new testApp());
}

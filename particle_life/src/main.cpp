
#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	//ofSetupOpenGL(1920,1080,OF_WINDOW);
	ofGLWindowSettings s;
	//s.setGLVersion(4, 3);
	//s.setSize(1920, 1080);
	s.setSize(1920, 1080);
	s.setPosition(glm::vec2(0, 0));
	ofCreateWindow(s);

	ofRunApp(new ofApp());

}

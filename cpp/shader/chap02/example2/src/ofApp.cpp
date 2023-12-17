#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	/////////////////////////////////////////////////////////////////////////////////
	// Version 1 :
	m_triangle.addVertex( glm::vec3(0.0,      0.0, 0.0) );	    // vertex 0
	m_triangle.addVertex( glm::vec3(0.0,    768.0, 0.0) );      // vertex 1
	m_triangle.addVertex( glm::vec3(1024.0, 768.0, 0.0) );      // vertex 2

	m_shader.load("first_vertex.vert" , "first_fragment.frag");
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	m_shader.begin();
	m_triangle.draw();
	m_shader.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

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
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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
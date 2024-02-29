#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	/////////////////////////////////////////////////////////////////////////////////
	// Version 3 : add color into the corresponding vertex
	m_triangle.addVertex( glm::vec3(-1.0,  1.0, 0.0) );		 // vertex #0
	m_triangle.addVertex( glm::vec3(-1.0, -1.0f, 0.0) );     // vertex #1
	m_triangle.addVertex( glm::vec3( 1.0, -1.0f, 0.0) );     // vertex #2

	m_triangle.addColor( ofFloatColor(1.0, 0.0f, 0.0f, 1.0f) ); // set vertex #0 color as red
	m_triangle.addColor( ofFloatColor(0.0, 1.0f, 0.0f, 1.0f) ); // set vertex #1 color as green
	m_triangle.addColor( ofFloatColor(0.0, 0.0f, 1.0f, 1.0f) ); // set vertex #2 color as blue

    /******************************************************************************
     * Or Use the following Code ( position and color alternatively ) :

	// vertex #0
	m_triangle.addVertex( glm::vec3(-1.0,  1.0, 0.0) );         // set vertex #0 position
	m_triangle.addColor( ofFloatColor(1.0, 0.0f, 0.0f, 1.0f) ); // set vertex #0 color as red

	// vertex #1
	m_triangle.addVertex( glm::vec3(-1.0, -1.0f, 0.0) );        // set vertex #1 position
	m_triangle.addColor( ofFloatColor(0.0, 1.0f, 0.0f, 1.0f) ); // set vertex #1 color as green

	// vertex #2
	m_triangle.addVertex( glm::vec3( 1.0, -1.0f, 0.0) );        // set vertex #2 position
	m_triangle.addColor( ofFloatColor(0.0, 0.0f, 1.0f, 1.0f) ); // set vertex #2 color as blue

     ***********************************************************************/

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
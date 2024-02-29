#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    //
    // set arguments of triangle
    //
    constexpr const int IDX_COUNT = 6;

    quad.addVertex( glm::vec3(-1, -1, 0) );  // downer - left
    quad.addVertex( glm::vec3(-1,  1, 0) );  // upper  - left
    quad.addVertex( glm::vec3( 1,  1, 0) );  // upper  - right
    quad.addVertex( glm::vec3( 1, -1, 0) );  // downer - right

    quad.addColor( ofDefaultColorType(1, 0, 0, 1) ); // red
    quad.addColor( ofDefaultColorType(0, 1, 0, 1) ); // green
    quad.addColor( ofDefaultColorType(0, 0, 1, 1) ); // blue
    quad.addColor( ofDefaultColorType(1, 1, 1, 1) ); // white

    ofIndexType indices[IDX_COUNT] = 
    { 
        0, 1, 2,  // Upper-Left   triangle
        2, 3, 0   // Downer-Right triangle
    };
    quad.addIndices( indices, IDX_COUNT);
    

    
    //
    // set shader script into shader program
    //
    shader.load("first_vertex.vert", "first_fragment.frag");
    
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    shader.begin();
      quad.draw();
    shader.end();
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

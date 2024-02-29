#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::buildMesh(ofMesh& mesh, float w, float h, glm::vec3 pos) {
    float verts[] = {
        // Left-Bottom
        -w + pos.x,
        -h + pos.y,
        pos.z,

        // Left-Upper
        -w + pos.x,
         h + pos.y,
        pos.z,

        // Right-Upper
         w + pos.x,
         h + pos.y,
         pos.z,

         // Right-Bottom
         w + pos.x,
        -h + pos.y,
         pos.z
    };

    float uvs[] = {
        0,0,
        0,1,
        1,1,
        1,0
    };
    
    for( int i = 0; i < 4; ++i ) {
        int idx   = i * 3;
        int uvIdx = i * 2;

        mesh.addVertex( glm::vec3(verts[idx], verts[idx + 1], verts[idx + 2]) );
        mesh.addTexCoord( glm::vec2(uvs[uvIdx], uvs[uvIdx + 1]) );
    }

    ofIndexType indices[6] = { 0,1,2,  2,3,0 };
    mesh.addIndices( indices, 6);
}


//--------------------------------------------------------------
void ofApp::setup(){
    buildMesh( charMesh, 0.25, 0.5, glm::vec3(0.0, 0.15, 0.0) );


    charShader.load("passthrough.vert", "alphaTest.frag");

    // openFrameworks 默认使用像索坐标而不是UV坐标的纹理类型，而我们希望使用更标准的纹理类型，因此需要禁用这部分功能
    ofDisableArbTex();
    // ofDisableAlphaBlending();
    alienImg.load("alien.png");
    

}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    // using namespace glm;

    charShader.begin();

    charShader.setUniformTexture("greenMan", alienImg, 0);
    charMesh.draw();

    //charShader.setUniformTexture("tex", bgnd, 0);
    //background.draw();

    charShader.end();

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

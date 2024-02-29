#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    //
    // set arguments of triangle
    //
    constexpr const int IDX_COUNT = 6;

    quad.addVertex( glm::vec3(-1, -1, 0) );  // #0 : downer - left
    quad.addVertex( glm::vec3(-1,  1, 0) );  // #1 : upper  - left
    quad.addVertex( glm::vec3( 1,  1, 0) );  // #2 : upper  - right
    quad.addVertex( glm::vec3( 1, -1, 0) );  // #3 : downer - right

    quad.addColor( ofDefaultColorType(1, 0, 0, 1) ); // #0 : red
    quad.addColor( ofDefaultColorType(0, 1, 0, 1) ); // #1 : green
    quad.addColor( ofDefaultColorType(0, 0, 1, 1) ); // #2 : blue
    quad.addColor( ofDefaultColorType(1, 1, 1, 1) ); // #3 : white

    //                                                 Blue   Alpha
    //                                                   |      |
    //                                                   |      |
    // file uv_vis.frag :     outColor = vec4( fragUV,  0.0f, 1.0f); 
    // 
    quad.addTexCoord( glm::vec2(0, 0) );     // #0 : downer - left   |  color : (0,0,0,1) -> Black    (0,0)  =>  red:0%   , green:0%    ( the blue:0%,  alpha:100% )
    quad.addTexCoord( glm::vec2(0, 1) );     // #1 : upper  - left   |  color : (0,1,0,1) -> Green    (0,1)  =>  red:0%   , green:100%  ( the blue:0%,  alpha:100% )
    quad.addTexCoord( glm::vec2(1, 1) );     // #2 : upper  - right  |  color : (1,1,0,1) -> Yellow   (1,1)  =>  red:100% , green:100%  ( the blue:0%,  alpha:100% )
    quad.addTexCoord( glm::vec2(1, 0) );     // #3 : downer - right  |  color : (1,0,0,1) -> Red      (1,0)  =>  red:100% , green:0%    ( the blue:0%,  alpha:100% )

    ofIndexType indices[IDX_COUNT] = 
    { 
        0, 1, 2,  // Upper-Left   triangle
        2, 3, 0   // Downer-Right triangle
    };
    quad.addIndices( indices, IDX_COUNT);
    
    // set shader script into shader program
    shader.load("uv_passthrough.vert", "uv_brightness.frag");

    // openFrameworks 默认使用像索坐标而不是UV坐标的纹理类型，而我们希望使用更标准的纹理类型，因此需要禁用这部分功能
    ofDisableArbTex();
    // ofDisableAlphaBlending();
    imageParrot.load("parrot.png");
    imageGrid.load("checker.jpg");
    
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    shader.begin();
    // 
    // 如果着色器使用多个纹理，则纹理位置很重要。如果为着色器设置了多个纹理统一变量，则必须确保每个纹理都被指定了不同的纹理位置。
    //                                           0 : 纹理位置 
    //
    /***********************************************************************************************************************************
    
       void ofShader::setUniformTexture(const string & name, const ofBaseHasTexture& img, int textureLocation)  const {
	        setUniformTexture(name, img.getTexture(), textureLocation);
        }
    ************************************************************************************************************************************/
    shader.setUniformTexture("parrotTex", imageParrot, 0);
    shader.setUniformTexture("checkerboardTex", imageGrid, 1);
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
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
    buildMesh( charMesh,  1/10.0f, 2.0f/10,  glm::vec3(0.0, -0.2, 0.0) );
    buildMesh( bgMesh,    1.0, 1.0,          glm::vec3(0.0, 0.0, 0.5) );
    buildMesh( cloudMesh, 1.0/4, 1.0f/6,     glm::vec3(-0.55, 0.0, 0.0) );
    buildMesh( sunlightMesh, 1.0, 1.0,       glm::vec3(0.0, 0.0, 0.4) );


    //printf("1: %s\n", glGetString(GL_VENDOR));               //返回负责当前OpenGL实现厂商的名字
    //printf("2: %s\n", glGetString(GL_RENDERER) );            //返回一个渲染器标识符，通常是个硬件平台
    //printf("3: %s\n", glGetString(GL_VERSION) );             //返回当前OpenGL实现的版本号
    //printf("4: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION) );//返回着色预压编译器版本号
    // 
    /*
       printf("5: %s\n", glGetString(GLU_VERSION) ); //返回当前GLU工具库版本 
    */


    // openFrameworks 默认使用像索坐标而不是UV坐标的纹理类型，而我们希望使用更标准的纹理类型，因此需要禁用这部分功能
    ofDisableArbTex();
    ofEnableDepthTest();
    // ofDisableAlphaBlending();


    alienImg.load("alien.png");
    backgroundImg.load("forest.png");
    cloudImg.load("cloud.png");
    sunlightImg.load("sun.png");

    alphaTestShader.load("passthrough.vert", "alphaTest.frag");
    cloudShader.load("passthrough.vert", "cloud.frag");

}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofDisableAlphaBlending();
    ofEnableDepthTest();

    alphaTestShader.begin();
        alphaTestShader.setUniformTexture("tex", alienImg, 0);
        charMesh.draw();

        alphaTestShader.setUniformTexture("tex", backgroundImg, 0);
        bgMesh.draw();
    alphaTestShader.end();



    ofEnableBlendMode( ofBlendMode::OF_BLENDMODE_ALPHA );
    ofDisableDepthTest();

    cloudShader.begin();
            cloudShader.setUniformTexture("tex", cloudImg, 0);
            cloudMesh.draw();

            ofEnableBlendMode( ofBlendMode::OF_BLENDMODE_ADD );
            cloudShader.setUniformTexture("tex", sunlightImg, 0 );
            sunlightMesh.draw();
    cloudShader.end();


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

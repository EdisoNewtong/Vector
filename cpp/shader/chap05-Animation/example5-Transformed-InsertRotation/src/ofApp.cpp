#include "ofApp.h"

void buildMesh(ofMesh& mesh, float w, float h, glm::vec3 pos)
{
	float verts[] = { -w + pos.x, -h + pos.y,  pos.z,
		-w + pos.x,  h + pos.y,  pos.z,
		w + pos.x,  h + pos.y,  pos.z,
		w + pos.x, -h + pos.y,  pos.z };

	float uvs[] = { 0,0, 0,1, 1,1, 1,0 };

	for (int i = 0; i < 4; ++i) {
		int idx = i * 3;
		int uvIdx = i * 2;

		mesh.addVertex(glm::vec3(verts[idx], verts[idx + 1], verts[idx + 2]));
		mesh.addTexCoord(glm::vec2(uvs[uvIdx], uvs[uvIdx + 1]));
	}

	ofIndexType indices[6] = { 0,1,2,2,3,0 };
	mesh.addIndices(indices, 6);
}

glm::mat4 buildMatrix( glm::vec3 trans, float rot, glm::vec3 scale )
{
	using glm::mat4;

	mat4 translation = glm::translate( trans );
	//                  rotate    'rot' (rad)   along  the Z-axis   
	mat4 rotation    = glm::rotate( rot,        glm::vec3(0.0, 0.0, 1.0) );
	mat4 scaler      = glm::scale( scale );

	return translation * rotation * scaler;

}


//--------------------------------------------------------------
void ofApp::setup()
{
	startRotateFlag = false;
    walkRight = false;
    charPos = glm::vec3(0.0, 0.0, 0.0);


	ofDisableArbTex();
	ofEnableDepthTest();

	buildMesh(charMesh, 0.1, 0.2, glm::vec3(0.0, -0.2, 0.0));
	buildMesh(backgroundMesh, 1.0, 1.0, glm::vec3(0.0, 0.0, 0.5));
	buildMesh(sunMesh, 1.0, 1.0, glm::vec3(0.0, 0.0, 0.4));
	buildMesh(cloudMesh, 0.25, 0.15, glm::vec3(0.0, 0.0, 0.0));

	alienImg.load("walk_sheet.png");
	backgroundImg.load("forest.png");
	cloudImg.load("cloud.png");
	sunImg.load("sun.png");

	spritesheetShader.load("spritesheet.vert", "alphaTest.frag");
	alphaTestShader.load("passthrough.vert", "alphaTest.frag");
	cloudShader.load("cloud.vert", "cloud.frag");
}

//--------------------------------------------------------------
void ofApp::update() {
    if ( walkRight ) {
        float speed = 0.4 * ofGetLastFrameTime();
        charPos += glm::vec3(speed, 0, 0);
    }

}

//--------------------------------------------------------------
void ofApp::draw() {
	

	static float frame = 0.0;
	frame = (frame > 10) ? 0.0 : frame += 0.2;
	glm::vec2 spriteSize = glm::vec2(0.28, 0.19);
	glm::vec2 spriteFrame = glm::vec2((int)frame % 3, (int)frame / 3);


	ofDisableBlendMode();
	ofEnableDepthTest();

	spritesheetShader.begin();
	spritesheetShader.setUniform2f("size", spriteSize);
	spritesheetShader.setUniform2f("offset", spriteFrame);
	spritesheetShader.setUniformTexture("tex", alienImg, 0);
    // update alien's position
	spritesheetShader.setUniform3f("deltaPos", charPos);

	charMesh.draw();

	spritesheetShader.end();

	alphaTestShader.begin();
	alphaTestShader.setUniformTexture("tex", backgroundImg, 0);
	backgroundMesh.draw();
	alphaTestShader.end();

	ofDisableDepthTest();
	ofEnableBlendMode(ofBlendMode::OF_BLENDMODE_ALPHA);

	cloudShader.begin();
	cloudShader.setUniformTexture("tex", cloudImg, 0);

	static float rotation = 0.0f;

	// glm::mat4 transformA = buildMatrix( glm::vec3(-0.55, 0.0, 0.0), rotation, glm::vec3(1.5, 1, 1) );

	//
	glm::mat4 translationA = glm::translate( glm::vec3(-0.55, 0.0, 0.0) );
	//                                  45 degree
	float alpha = 3.14f/4 + (!startRotateFlag ? rotation : 0.0);
	glm::mat4 rotationA    = glm::rotate(alpha,        glm::vec3(0.0, 0.0, 1.0) );
	// rotationA += rotation;

	glm::mat4 scaleA       = glm::scale( glm::vec3(1.5, 1, 1) );
	// glm::mat4 transformA   = translationA * scaleA;
	glm::mat4 transformA   = translationA * rotationA * scaleA;

	if ( startRotateFlag ) {
		rotation += 0.1f;
	}

	glm::mat4 ourRotation = glm::rotate( rotation, glm::vec3(0.0, 0.0, 1.0) );
	glm::mat4 newMatrix   = translationA * ourRotation * glm::inverse(translationA);
	
	glm::mat4 finalMatrixA = transformA; 
	if ( startRotateFlag ) {
		/*
		* 
		finalMatrixA = (translationA * ourRotation * glm::inverse(translationA) ) * ( translationA * rotationA * scaleA );

		               <==========================================================<=======================================
		*/
		finalMatrixA = newMatrix * transformA;
	}
	
	glm::mat4 transformB = buildMatrix( glm::vec3(  0.4, 0.2, 0.0), 1.0f, glm::vec3(1  , 1, 1) );

	cloudShader.setUniformMatrix4f("transform", finalMatrixA  );
	cloudMesh.draw();

	cloudShader.setUniformMatrix4f("transform", transformB  );
	cloudMesh.draw();


	//ofEnableBlendMode(ofBlendMode::OF_BLENDMODE_ADD);
	//cloudShader.setUniformTexture("tex", sunImg, 0);
	//sunMesh.draw();

	cloudShader.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if ( key == ofKey::OF_KEY_RIGHT ) { 
        walkRight = true;
    } else if ( key == ofKey::OF_KEY_UP ) {
		startRotateFlag = true;
	}

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
    if ( key == ofKey::OF_KEY_RIGHT ) { 
        walkRight = false;
    } else if ( key == ofKey::OF_KEY_UP ) {
		startRotateFlag = false;
	}

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

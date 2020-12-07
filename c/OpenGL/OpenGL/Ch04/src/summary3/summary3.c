#include <stdlib.h>
#include <math.h>

// Apple Mac OX , OpenGL header files
#include <GLUT/glut.h>

const GLdouble twoPi = 6.283185;

class srcPt { 
public:
	GLint x;
	GLint y;
};

GLsizei winWidth = 400;
GLsizei winHeight = 300;

void init(void)
{
	glClearColor(1.0,1.0,1.0,1.0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, 200.0, 0.0, 150.0);
}

void pieChart(void)
{
	srcPt circCtr, piePt;
	GLint radius = winWidth/4;

	GLdouble sliceAngle,previousSliceAngle = 0.0;
	GLint k, nSlices = 12;
	GLfloat dataValues[12] = {
		10.0, 7.0, 13.0, 5.0, 13.0, 14.0,
		3.0, 16.0, 5.0, 3.0, 17.0, 8.0
	};
	GLfloat dataSum = 0.0;
	circCtr.x = winWidth/2;
	circCtr.y = winHeight/2;
	circleMidPoint(circCtr,radius);	// Call a midpoint circle-plot routine.

	for(k = 0; k < nSlices; ++k) {
		dataSum += dataValues[k];
	}

	for(k = 0; k < nSlices; ++k) {
		sliceAngle = twoPi * dataValues[k] / dataSum + previousSliceAngle;
		piePt.x = circCtr.x + radius * cos(sliceAngle);
		piePt.y = circCtr.x + radius * sin(sliceAngle);
		glBegin(GL_LINES);
			glVertex2i(circCtr.x, circCtr.y);
			glVertex2i(piePt.x, piePt.y);
		glEnd();
		previousSliceAngle = sliceAngle;
	}
}

int main(int argc, char* argv[]) {

	return 0;
}


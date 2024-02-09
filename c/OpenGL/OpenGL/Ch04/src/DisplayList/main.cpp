#include <iostream>
#include <math.h>
#include <stdlib.h>


// Apple Mac OX , OpenGL header files
#if defined(__GNUC__) // Linux Part
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#else                // !(Linux) Part
#include <GLUT/glut.h>
#include <GLUT/freeglut.h>
#endif


using namespace std;

const double TWO_PI = 6.2831853;

GLsizei winWidth = 400;
GLsizei winHeight = 400;

GLuint G_regID = 0;

class Point
{
public:
	Point() :x(0), y(0) {
	}

	void set_XY(GLint xVal,GLint yVal)
	{
		x = xVal; 
		y = yVal;
	}

	GLint getX() { return this->x; }
	GLint getY() { return this->y; }
private:
	GLint x;
	GLint y;
};

static void init()
{
	Point vertex;
	Point centerPt;
	GLdouble theta = 0.0;
	GLint k = 0;

	centerPt.set_XY( winWidth/2, winHeight/2);

	glClearColor(1.0,1.0,1.0,0.0);

	// 半径 150
	const int radius = 150;

	G_regID = glGenLists(1);
	glNewList(G_regID, GL_COMPILE);
		// set color as Red
		glColor3f(1.0,0.0,0.0);
		
		// 绘制一个正6边形， 6边形的中心在 屏幕中心，6边形的半径是150
		glBegin(GL_POLYGON);
			for(k = 0; k < 6; ++k) {
				theta = TWO_PI * k / 6.0;
				vertex.set_XY( centerPt.getX() + radius * cos(theta),
							   centerPt.getY() + radius * sin(theta) );

				glVertex2i( vertex.getX() , vertex.getY() );
			}
		glEnd();
	glEndList();

}


void regHexagon()
{
	// static int cnt = 0;
	// ++cnt;
	// cout << "Draw 6 Hexagon , " << cnt << " times " << endl;

	glClear(GL_COLOR_BUFFER_BIT);
	glCallList(G_regID);
	glFlush();
}

void winReshapeFunc(int newW,int newH)
{
	// static int cnt = 0;
	// ++cnt;
	// cout << "Reshape , " << cnt << " times " << endl;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)newW, 0.0, (GLdouble)newH);

	// winWidth = (GLsizei)newW;
	// winHeight = (GLsizei)newH;

	glClear( GL_COLOR_BUFFER_BIT);
}

int main(int argc,char** argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode( GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(winWidth, winHeight);

	glutCreateWindow("Reshape-Function & Display-List Example");
	
	init();
	glutDisplayFunc( regHexagon );
	glutReshapeFunc( winReshapeFunc);

	glutMainLoop();

	return 0;
}


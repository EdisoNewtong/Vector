#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

GLsizei winWidth  = 1000;
GLsizei winHeight = 1000;

GLfloat xwcMin = -500.0f;
GLfloat xwcMax = 500.0f;
GLfloat ywcMin = -500.0f;
GLfloat ywcMax = 500.0f;


void init(void)
{
	glClearColor(0.0,0.0,0.0, 0.0);

	// glMatrixMode(GL_MODELVIEW);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(xwcMin, xwcMax, ywcMin, ywcMax);
}

void displayFcn(void)
{
	// printf("In displayFcn(...)\n");
	// glMatrixMode(GL_PROJECTION);
 	// glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	// glLoadIdentity();

	// 0. Orignal Rect
	// glLoadIdentity();
	glColor3f(0.0, 0.0, 1.0);
	glRecti(50, 100, 200, 150);
	glFlush();

	// 1. Translate
	glColor3f(1.0, 0.0, 0.0);
	glTranslatef(-200,-50.0, 0.0);
	glRecti(50, 100, 200, 150);

	// 2. Rotate
	glLoadIdentity();
	glColor3f(0.0, 1.0, 0.0);
	glRotatef(90.0, 0.0, 0.0, 1.0);
	glRecti(50, 100, 200, 150);

	// 3. Scale
	// glLoadIdentity();
	glColor3f(1.0, 0.0, 1.0);
	glScalef(-0.5, 1.0, 1.0);
	glRecti(50, 100, 200, 150);

	glFlush();
}

void winReshapeFcn(GLint newWidth, GLint newHeight)
{
	// printf("In winReshapeFcn(...)\n");
	// glClearColor(1.0,1.0,0.0, 0.0);
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	gluOrtho2D(xwcMin, xwcMax, ywcMin, ywcMax);

	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// glFlush();
}

int main(int argc,char* argv[])
{
	glutInit(&argc,argv);
	glutInitDisplayMode( GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(50,50);
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow("Geometric Transformation Sequence");

	init();
	glutDisplayFunc(displayFcn);
	glutReshapeFunc(winReshapeFcn);

	glutMainLoop();
	return 0;
}

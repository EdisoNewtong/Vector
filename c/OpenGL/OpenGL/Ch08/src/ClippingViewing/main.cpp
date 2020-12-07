#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

GLint winWidth = 600;
GLint winHeight = 300;

GLfloat xwcMin = -100.0f; 
GLfloat xwcMax =  100.0f;
GLfloat ywcMin = -100.0f;
GLfloat ywcMax =  100.0f;


class wcPt2D {
public:
	GLfloat x;
	GLfloat y;
};

void init(void)
{
	glClearColor(0.0,0.0,0.0, 0.0);

	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(xwcMin, xwcMax, ywcMin, ywcMax);

	glMatrixMode(GL_MODELVIEW);
}

void triangle(wcPt2D* verts)
{
	GLint k = 0;
	glBegin(GL_TRIANGLES);
		for(k = 0; k < 3; ++k){
			glVertex2f(verts[k].x ,verts[k].y);
		}
	glEnd();
}

void displayFcn(void)
{
	// printf("In displayFcn(...)\n");
	wcPt2D verts[3] = {
		{ -50.0f, -25.0f },
		{  50.0f, -25.0f },
		{   0.0f,  50.0f },
	};

	glClear(GL_COLOR_BUFFER_BIT);

	// Blue Triangle
	glColor3f(0.0,0.0,1.0);
	glViewport(0,0,300,300);
	triangle(verts);

	// Red Triangle
	glColor3f(1.0,0.0,0.0);
	glViewport(300,0,300,300);
	glRotatef(90.0,0.0,0.0,1.0);
	triangle(verts);
	
	glFlush();
}

void winReshapeFcn(GLint newWidth, GLint newHeight)
{
	// printf("In winReshapeFcn(...)\n");
	// glClearColor(1.0,1.0,0.0, 0.0);
	// glMatrixMode(GL_MODELVIEW);
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	GLfloat xScale = (newWidth * 1.0f /winWidth);
	GLfloat yScale = (newHeight* 1.0f /winHeight);
	gluOrtho2D(xwcMin * xScale, xwcMax * xScale, ywcMin * yScale, ywcMax * yScale);
	// gluOrtho2D(xwcMin, xwcMax, ywcMin, ywcMax);

	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	winWidth = newWidth;
	winHeight = newHeight;

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

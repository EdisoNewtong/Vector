#include <stdio.h>

// Apple Mac OX , OpenGL header files
#if defined(__GNUC__) // Linux Part
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#else                // !(Linux) Part
#include <GLUT/glut.h>
#include <GLUT/freeglut.h>
#endif



GLsizei winWidth = 600;
GLsizei winHeight = 500;
GLint xRaster = 25;
GLint yRaster = 150;

GLubyte label[36] = {
	'J', 'a' , 'n',
	'F', 'e' , 'b',
	'M', 'a' , 'r',
	'A', 'p' , 'r',
	'M', 'a' , 'y',
	'J', 'u' , 'n',
	'J', 'u' , 'l',
	'A', 'u' , 'g',
	'S', 'e' , 'p',
	'O', 'c' , 't',
	'N', 'o' , 'v',
	'D', 'e' , 'c'
};

GLint dataValue[12] = { 
	420,
	342,
	324,
	310,
	262,
	185,
	190,
	196,
	217,
	240,
	312,
	438
};

void init()
{
	glClearColor(1.0,1.0,1.0,1.0);	// White display window
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, 600.0, 0.0, 500.0);
}

void lineGraph()
{
	GLint month,k;
	GLint x = 30;	// Initialize x position for chart.

	glClear(GL_COLOR_BUFFER_BIT);	// Clear display window
	glColor3f(0.0,0.0,1.0);			// Set line color to blue

	const int delta_X = 50;
	glBegin(GL_LINE_STRIP);
		for(k = 0; k < 12; ++k) {
			glVertex2i(x + k*delta_X, dataValue[k]);
		}
	glEnd();

	// Draw 12 stars
	xRaster = 25;
	glColor3f(1.0,0.0,0.0);	// setmarker color to red;
	for( k = 0; k < 12; ++k) {
		glRasterPos2i(xRaster + k * delta_X, dataValue[k] - 4);
		// glRasterPos2i(x + k * delta_X, dataValue[k] - 4);
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '*');
	}

	// Draw 12 months characters
	glColor3f(0.0, 0.0, 0.0);
	xRaster = 20;
	for( month = 0; month < 12; ++month) {
		glRasterPos2i(xRaster, yRaster);
		for(k = 3*month; k < 3*month + 3; ++k) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, label[k]);
		}
		xRaster += 50;
	}

	glFlush();
}

void winReshapeFunc(GLint newWidth, GLint newHeight)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)newWidth, 0.0, (GLdouble)newHeight);

	glClear(GL_COLOR_BUFFER_BIT);
}

int main(int argc,char* argv[])
{
	glutInit(&argc,argv);
	glutInitDisplayMode( GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow("Line Chart Data");

	init();
	glutDisplayFunc(lineGraph);
	glutReshapeFunc(winReshapeFunc);

	glutMainLoop();
    return 0;
}


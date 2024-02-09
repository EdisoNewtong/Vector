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


static const float vertex_list[][3] = {
    { -0.5f, -0.5f, -0.5f },
    { 0.5f, -0.5f, -0.5f  },
	{-0.5f,  0.5f, -0.5f  },
	{ 0.5f,  0.5f, -0.5f  },

    { -0.5f, -0.5f,  0.5f },
	{ 0.5f,  -0.5f,  0.5f },
	{ -0.5f,  0.5f,  0.5f },
	{  0.5f,  0.5f,  0.5f }
};

static const GLint index_list[][2] = {
	{0, 1},
	{2, 3},
	{4, 5},
	{6, 7},
	{0, 2},
	{1, 3},
	{4, 6},
	{5, 7},
	{0, 4},
	{1, 5},
	{7, 3},
	{2, 6}
};

static float rotate = 0.0f;
static int times = 0;

void DrawCube()
{

	glBegin(GL_LINES);
	for(int i = 0; i < 12; ++i) {
		for(int j = 0; j < 2; ++j) {
			glVertex3fv(vertex_list[index_list[i][j]]);
		}
	}
	glEnd();
}

void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();


	times++;
    if(times > 100)
    {
        times = 0;
    }
 
    if(times % 100 == 0)
    {
        // rotate += 0.3;
        rotate += 3.5f;
    }
    
    glRotatef(rotate, 0, 1, 0);
    glRotatef(rotate, 1, 0, 0);

	glColor3f(0,0,1);

	DrawCube();

	glPopMatrix();
	glutSwapBuffers();
}

int main(int argc,char* argv[])
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(500,500);
	glutCreateWindow("An Example OpenGL Program");

	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutMainLoop();

	return 0;
}



#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

GLsizei winWidth  = 600;
GLsizei winHeight = 600;

GLfloat xwcMin = 0.0f;
GLfloat xwcMax = 225.0f;
GLfloat ywcMin = 0.0f;
GLfloat ywcMax = 225.0f;

class wcPt2D {
public:
	GLfloat x;
	GLfloat y;
};

#define  MROW_MAX  3
#define  MCOL_MAX  3

typedef GLfloat Matrix3x3[MROW_MAX][MCOL_MAX];

Matrix3x3 matComposite;

const GLdouble pi = 3.14159265;

void init(void)
{
	glClearColor(1.0,1.0,1.0, 0.0);
}

void matrix3x3SetIdentity(Matrix3x3 matIdent3x3)
{
	GLint row = 0;
	GLint col = 0;

	for(row = 0; row < MROW_MAX; ++row) 
	{
		for(col = 0; col < MCOL_MAX; ++col) 
		{
			matIdent3x3[row][col] = (row==col ? 1.0f : 0.0f);
		}
	}
}

// m1*m2 --> m2
void matrix3x3PreMultipy(Matrix3x3 m1, Matrix3x3 m2)
{
	GLint row = 0;
	GLint col = 0;
	Matrix3x3 matTemp = {
		{ 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f }
	};

	for(row = 0; row < MROW_MAX; ++row) 
	{
		for(col = 0; col < MCOL_MAX; ++col) 
		{
			matTemp[row][col] =   m1[row][0] * m2[0][col]
				                + m1[row][1] * m2[1][col]
				                + m1[row][2] * m2[2][col];
		}
	}

	for(row = 0; row < MROW_MAX; ++row) 
	{
		for(col = 0; col < MCOL_MAX; ++col) 
		{
			m2[row][col] =  matTemp[row][col];
		}
	}
}

//
// 平移 变换
//
void translate2D(GLfloat tx, GLfloat ty)
{
	Matrix3x3 matTrans1;

	matrix3x3SetIdentity(matTrans1);

	matTrans1[0][2] = tx;
	matTrans1[1][2] = ty;

	matrix3x3PreMultipy(matTrans1, matComposite);
}

//
// 旋转 变换
//
void rotate2D(wcPt2D pivotPt, GLfloat theta)
{
	Matrix3x3 matRot;

	matrix3x3SetIdentity(matRot);

	matRot[0][0] = cos(theta);
	matRot[0][1] = -sin(theta);
	matRot[0][2] = pivotPt.x * (1-cos(theta)) + pivotPt.y * sin(theta);

	matRot[1][0] = sin(theta);
	matRot[1][1] = cos(theta);
	matRot[1][2] = pivotPt.y * (1-cos(theta)) - pivotPt.x * sin(theta);

	matrix3x3PreMultipy(matRot, matComposite);
}

void scale2D(GLfloat sx, GLfloat sy, wcPt2D fixedPt)
{
	Matrix3x3 matScale;

	matrix3x3SetIdentity(matScale);

	matScale[0][0] = sx;
	matScale[0][2] = (1-sx) * fixedPt.x;
	matScale[1][1] = sy;
	matScale[1][2] = (1-sy) * fixedPt.y;

	matrix3x3PreMultipy(matScale, matComposite);
}


void transformVerts2D(GLint nVerts, wcPt2D* verts)
{
	GLint k;
	GLfloat temp;

	//**************************************************
	// Homogeneous Coordinates  P226
	//**************************************************
	// verts[i] (x,y)  (x,y,1)
	//  --->
	//
	// Matrix 3*1
	//				| vert[i].x |
	// verts[i]		| vert[i].y	|
	//				|    1		|
	//
	// 3*3  X  3*1  ==>   3*1
	for(k = 0; k < nVerts; ++k) 
	{
		temp = matComposite[0][0] * verts[k].x
		     + matComposite[0][1] * verts[k].y
		     + matComposite[0][2];

		verts[k].y = matComposite[1][0] * verts[k].x
			       + matComposite[1][1] * verts[k].y
				   + matComposite[1][2];
		verts[k].x = temp;
	}
}


void triangle(wcPt2D* verts)
{
	GLint k;

	glBegin(GL_TRIANGLES);
		for(k = 0; k < 3; ++k) 
		{
			glVertex2f(verts[k].x, verts[k].y);
		}
	glEnd();

}


void displayFcn(void)
{
	GLint nVerts = 3;

	/*
	 *           (2)
	 *            #
	 *           # #
	 *          #   #
	 *         #     #
	 *        #       #
	 *       #         #
	 *  (0) ############# (1)
	 */
	wcPt2D verts[3] = {
		{ 50.0f,  25.0f },  // 0
		{ 150.0f, 25.0f },  // 1
		{ 100.0f, 100.0f }, // 2 
	};

	wcPt2D centroidPt;

	GLint k,xSum =0,ySum =0;
	for(k = 0; k < nVerts; ++k) {
		xSum += verts[k].x;
		ySum += verts[k].y;
	}

	centroidPt.x = GLfloat(xSum) / GLfloat(nVerts);
	centroidPt.y = GLfloat(ySum) / GLfloat(nVerts);

	wcPt2D pivPt, fixedPt;
	pivPt = centroidPt;
	fixedPt = centroidPt;

	GLfloat tx = 0.0f;
	GLfloat ty = 100.0f;
	GLfloat sx = 0.3f;
	GLfloat sy = 0.3f;
	GLdouble theta = pi/2.0;	// rotate 90 degree

	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0, 0.0, 1.0);
	triangle(verts);

	matrix3x3SetIdentity(matComposite);

	scale2D(sx, sy, fixedPt);
	rotate2D(pivPt, theta);
	translate2D(tx,ty);

	transformVerts2D(nVerts,verts);


	glColor3f(1.0, 0.0, 0.0);
	triangle(verts);

	glFlush();
}

void winReshapeFcn(GLint newWidth, GLint newHeight)
{
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	gluOrtho2D(xwcMin, xwcMax, ywcMin, ywcMax);

	glClear(GL_COLOR_BUFFER_BIT);
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

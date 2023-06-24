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

void init(void)
{
	// Set display-window color to Green
	glClearColor(0.0,0.0,0.0,0.0);

	// Set projection parameters
	glMatrixMode(GL_PROJECTION);
	// projection ranges : x:[0,500] , y:[0,150]
	// 与窗口大小没有关系，只会影响图元的显示
	// 图元的坐标在这个设置的范围之外，将无法显示 
	gluOrtho2D(0.0, 500.0, 0.0, 150.0);
}

void lineSegment(void)
{
	static int G_CNT = 0;
	++G_CNT;
	printf("%d. lineSegment\n", G_CNT);

	// Clear the display Window
	// 如果注释掉以下这句话，那么 glClearColor(0.0,1.0,0.0,0.0); 设置成绿色无效
	// 默认是 黑色
	glClear(GL_COLOR_BUFFER_BIT);


	// 设置要绘制的图元的颜色 
	glBegin(GL_LINES);	// 设置为绘制图元模式 , 直线为2个顶点分为1组
		// OpenGL Coordinate , left-bottom is the Original-Point
		// Line 1
		glColor3f(1.0,0.0,0.0);
		glVertex2i(10,10);
		glVertex2i(390,10);
		// Line 2
		glColor3f(1.0,1.0,0.0);
		glVertex2i(10, 20);
		glVertex2i(400, 20);
		// Line 3
		glColor3f(0.0,0.0,1.0);
		glVertex2i(10, 30);
		glVertex2i(490, 30);
		// Line 4
		glColor3f(1.0,1.0,1.0);
		glVertex2i(10, 40);
		glVertex2i(495, 40);
	glEnd();

	// 处理传入OpenGL的绘制命令 尽快的!!! 
	// as quickly as possible
	glFlush();
}

int main(int argc,char** argv)
{
	// 1st . init GLUT
	glutInit(&argc,argv);
	// Set Display Mode
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	// Set Window Postion / Size by Screen Coordinate
	glutInitWindowPosition(0,0);
	glutInitWindowSize(400,300);
	// 设置窗口 标题
	glutCreateWindow("An Example OpenGL Program");

	// 设置窗口的背景色，投影模式
	init();
	// 设置绘制函数
	glutDisplayFunc(lineSegment);
	// last. 最后 调用主循环
	glutMainLoop();

    return 0;
}

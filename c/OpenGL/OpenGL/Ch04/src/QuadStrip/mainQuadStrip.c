#include <stdio.h>

// Apple Mac OX , OpenGL header files
#include <GLUT/glut.h>

void init(void)
{
	// Set display-window color to Green
	glClearColor(0.0,0.0,0.0,0.0);

	// Set projection parameters
	glMatrixMode(GL_PROJECTION);
	// projection ranges : x:[0,500] , y:[0,150]
	// 与窗口大小没有关系，只会影响图元的显示
	// 图元的坐标在这个设置的范围之外，将无法显示 
	gluOrtho2D(0.0, 400.0, 0.0, 300.0);
}

void lineSegment(void)
{
	// static int G_CNT = 0;
	// ++G_CNT;
	// printf("%d. lineSegment\n", G_CNT);

	// Clear the display Window
	// 如果注释掉以下这句话，那么 glClearColor(0.0,1.0,0.0,0.0); 设置成绿色[无效]!!!
	// 默认是 黑色
	glClear(GL_COLOR_BUFFER_BIT);

	//
	// Anti-ClockWise	/  Counter-ClockWise
	//			*(p6)		*(p5)
	//
	//
	//
	//
	//   *(p1)							*(p4)
	//
	//
	//
	//
	//			*(p2)		*(p3)
	
	int p1[2] = { 10, 230 };
	int p2[2] = { 20, 110 };
	int p3[2] = { 100,110 };
	int p4[2] = { 105,200 };
	int p5[2] = { 140,200 };
	int p6[2] = { 140,125 };
	int p7[2] = { 250,85 };
	int p8[2] = { 250,230 };
	glColor3f(1.0,0.0,0.0);	// Set Rectagle Color as Red
	// 画多个连续4边形
	// 4个1组，
	//		第2组共享第1组最后2个顶点为公共边
	//		第3组共享第2组最后2个顶点为公共边
	//				   	
	// 8个顶点，依次是 p1,p2,p4,p3,p5,p6,p8,p7
	//
	// 每1分组的4个顶点的排序顺序依次是 2n-1,2n,2n+2,2n+1
	// e.g. 
	// 第1组: n=1 , 1,2,4,3 --> 对应8个顶点中的 p1,p2,p3,p4
	// 第2组: n=2 , 3,4,6,5 --> 对应8个顶点中的 p4,p3,p6,p5
	// 第3组: n=3 , 5,6,8,7 --> 对应8个顶点中的 p5,p6,p7,p8
	glBegin(GL_QUAD_STRIP);
		glVertex2iv(p1);	
		glVertex2iv(p2);	
		glVertex2iv(p4);
		glVertex2iv(p3);	
		glVertex2iv(p5);	
		glVertex2iv(p6);
		glVertex2iv(p8);
		glVertex2iv(p7);
	glEnd();

	// 处理传入OpenGL的绘制命令 尽快的!!! 
	// as quickly as possible
	glFlush();
}

void main(int argc,char** argv)
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
}






#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Apple Mac OX , OpenGL header files
#include <GLUT/glut.h>

class srcPt { 
public:
	GLint x;
	GLint y;
};

int main(int argc, char* argv[]) {
	srcPt pt;
	pt.x = 1;
	pt.y = 2;
	// cout << "pt.x = "  << pt.x << "\tpt.y = " << pt.y << endl;
	printf("(x,y) = (%d,%d)\n",pt.x,pt.y);

	return 0;
}


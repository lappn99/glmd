#include <stdio.h>
#include <GL/glut.h>

GLuint callList;

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	
	glCallList(callList);
	
	glFlush();
	glutSwapBuffers();
}

void triangle()
{
	glBegin(GL_TRIANGLES);
		glVertex3f(0, 1, 0.0);
		glVertex3f(-1, -1.0, 0.0);
		glVertex3f(1, -1.0, 0);
	glEnd();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE);	// Use single display buffer.
	glutInitWindowSize(300, 300);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Hello world");

	callList = glGenLists(1);
	glNewList(callList,GL_COMPILE);
	triangle();
	glEndList();

	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}

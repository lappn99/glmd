#include <stdio.h>
#include <GL/glut.h>

GLuint triangleList,quadList;

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	
	//glCallList(triangle);
	glCallList(quadList);
	glutSwapBuffers();
}

void triangle()
{
	glBegin(GL_TRIANGLES);
		glVertex3f(0, 0.25, 0.0);
		glVertex3f(-0.5, -0.25, 0.0);
		glVertex3f(0.25, -0.5, 0);
	glEnd();

}

void quad()
{
	glBegin(GL_QUADS);
		glVertex3f(-1,-1,0); //Bottom left
		glVertex3f(-1,1,0); 
		glVertex3f(1,1,0);
		glVertex3f(1,-1,0);
	glEnd();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE);	// Use single display buffer.
	glutInitWindowSize(300, 300);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Hello world");

	triangleList = glGenLists(1);
	glNewList(triangleList,GL_COMPILE);
	triangle();
	glEndList();
	quadList = glGenLists(1);
	glNewList(quadList,GL_COMPILE);
	quad();
	glEndList();

	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}

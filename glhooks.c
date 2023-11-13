#include <dlfcn.h> //dlsym, RTLD_NEXT
#include <GL/gl.h>
#include <stdio.h>
#include "glmd.h"

typedef void(*_glVertex3fFunc)(GLfloat, GLfloat, GLfloat);
typedef void(*_glBegin)(GLenum);


extern void
glVertex3f(GLfloat x, GLfloat y, GLfloat z)
{
    glmdAddVertex(x,y,z);
}

extern void
glBegin(GLenum mode)
{
    glmdBeginVtxList();
}

extern void
glEnd(void)
{
    glmdEndVtxList();
}

extern void
glFlush(void)
{
    
}


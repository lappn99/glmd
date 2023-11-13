#include <dlfcn.h> //dlsym, RTLD_NEXT
#include <GL/gl.h>
#include <stdio.h>
#include "glmd.h"

typedef void(*_glVertex3fFunc)(GLfloat, GLfloat, GLfloat);
typedef void(*_glBeginFunc)(GLenum);
typedef void(*_glFlushFunc)(void);

extern void
glVertex3f(GLfloat x, GLfloat y, GLfloat z)
{
    GLMDParam params[4];
    params[0].opv = GLMDOP_VTX3F;
    params[1].fv = x;
    params[2].fv = y;
    params[3].fv = z;
    glmdCmd(params,4);
}

extern void
glBegin(GLenum mode)
{
    GLMDParam params[1];
    params[0].opv = GLMDOP_START_CMDLIST;
    glmdCmd(params,1);
}

extern void
glEnd(void)
{
    glmdEndVtxList();
}

extern void
glFlush(void)
{
    
    _glFlushFunc newFunc = glmdGetFuncAddr("glFlush");
    newFunc();
    glmdDraw();
}


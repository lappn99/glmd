#include <dlfcn.h> //dlsym, RTLD_NEXT
#include <GL/gl.h>
#include <stdio.h>
#include "glmd.h"

typedef void(*_glVertex3fFunc)(GLfloat, GLfloat, GLfloat);
typedef void(*_glBeginFunc)(GLenum);
typedef void(*_glFlushFunc)(void);
typedef GLuint (*_glGenListsFunc)(GLsizei);
typedef void (*_glNewListFunc)(GLuint, GLenum);
typedef void (*_glEndListFunc)(void);

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
    GLMDParam params[2] = { 
        {.opv = GLMDOP_START_VTXLIST},
        {.uiv = mode}
    };
    
    glmdCmd(params,2);
}

extern void
glEnd(void)
{
    GLMDParam params[2] = { {.opv = GLMDOP_END_VTXLIST},{.opv = GLMDOP_DRAW} };
    glmdCmd(params,1);
    glmdCmd(&params[1],1);
    
    
}

extern void
glFlush(void)
{
    
    _glFlushFunc newFunc = glmdGetFuncAddr("glFlush");
    newFunc();
    GLMDParam params[1] = { {.opv = GLMDOP_FLUSH } };
    glmdCmd(params,1);
    
}

extern GLuint
glGenLists(GLsizei range)
{
    _glGenListsFunc newFunc = glmdGetFuncAddr("glGenLists");
    GLuint result = newFunc(range);
    GLMDParam params[2] = {
        {.opv = GLMDOP_CREATE_CMDLIST},
        {.uiv = result}
    };

    glmdCmd(params,2);
    return result;
}


extern void
glNewList(GLuint list, GLenum mode)
{
    
    GLMDParam params[2] = {
        {.opv = GLMDOP_START_CMDLIST},
        {.uiv = list}
    };
    glmdCmd(params,2);

}

extern void
glEndList(void)
{
    GLMDParam params[1] = {
        {.opv = GLMDOP_END_CMDLIST}
    };
    glmdCmd(params,1);
}

extern void
glCallList(GLuint name)
{
    GLMDParam params[2] = {
        {.opv = GLMDOP_EXEC_CMDLIST},
        {.uiv = name}
    };
    glmdCmd(params,2);
}

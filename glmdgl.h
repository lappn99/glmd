#ifndef _GLMDGL_H
#define _GLMDGL_H
#include <GL/glext.h>



typedef struct GL
{   
    PFNGLBINDBUFFERPROC glBindBuffer;
    PFNGLGENBUFFERSPROC glGenBuffers;


} GL;

void glmdInitGL(GL* gl);

#endif //_GLMDGL_H
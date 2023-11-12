#ifndef _GLMDGL_H
#define _GLMDGL_H
#include <GL/glext.h>



typedef struct GL
{   
    int initialized;
    PFNGLBINDBUFFERPROC glBindBuffer;
    PFNGLGENBUFFERSPROC glGenBuffers;
    PFNGLBUFFERDATAPROC glBufferData;


} GL;

void glmdInitGL(GL* gl);

#endif //_GLMDGL_H
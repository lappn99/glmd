#include <dlfcn.h> //dlopen, dlsym
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include "glmdgl.h"


void
glmdInitGL(GL* gl)
{
    if(gl->initialized)
    {
        return;
    }
    void* libgl = dlopen("libGL.so",RTLD_LAZY);
    gl->glBindBuffer= dlsym(libgl,"glBindBuffer");
    gl->glGenBuffers= dlsym(libgl,"glGenBuffers");
    gl->glBufferData = dlsym(libgl,"glBufferData");
    gl->glDrawArrays = dlsym(libgl,"glDrawArrays");
    
}

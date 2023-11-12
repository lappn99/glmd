#include <dlfcn.h> //dlopen, dlsym
#include <stdio.h>
#include <GL/gl.h>
#include "glmdgl.h"


void
glmdInitGL(GL* gl)
{
    void* libgl = dlopen("libGL.so",RTLD_LAZY);
    gl->glBindBuffer= dlsym(libgl,"glBindBuffer");
    gl->glGenBuffers= dlsym(libgl,"glGenBuffers");
}

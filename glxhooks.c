#include <dlfcn.h> //dlsym
#include <GL/glx.h>
#include <GL/glext.h>
#include <stdio.h>
#include <string.h> //strcmp
#include "glmd.h"


typedef void (*glxFuncPointer)(void);
typedef Bool (*_glXMakeCurrentFunc)(Display*, GLXDrawable, GLXContext );
typedef Bool (*_glXMakeContextCurrentFunc)(Display*, GLXDrawable, GLXDrawable, GLXContext );
typedef GLXContext (*_glXCreateContextFunc)(Display*, XVisualInfo*, GLXContext, Bool);
typedef GLXContext (*_glXCreateNewContextFunc)(Display*, GLXFBConfig,int , GLXContext, Bool);
typedef glxFuncPointer (*_glXGetProcAddressFunc)(const GLubyte*);


extern Bool 
glXMakeCurrent(Display* dsp, GLXDrawable draw, GLXContext ctx)
{
    _glXMakeCurrentFunc newFunc = glmdGetFuncAddr("glXMakeCurrent");
   
    Bool result = newFunc(dsp,draw,ctx);
    if(result) 
    {
        glmdMakeContextCurrent();
    }
    return result;
    

}

extern Bool 
glXMakeContextCurrent(Display* dsp, GLXDrawable draw,GLXDrawable read, GLXContext ctx)
{
    
    _glXMakeContextCurrentFunc newFunc = glmdGetFuncAddr("glXMakeContextCurrent");
    Bool result = newFunc(dsp,read,draw,ctx);
    if(result)
    {
        glmdMakeContextCurrent();
    }
   
    return result;

}

extern GLXContext 
glXCreateContext(Display* dpy, XVisualInfo* vis, GLXContext shareList, Bool direct)
{
    _glXCreateContextFunc newFunc = glmdGetFuncAddr("glXCreateContext");
    glmdCreateContext();
    return newFunc(dpy, vis, shareList, direct);
}

extern GLXContext 
glXCreateNewContext(Display* dpy, GLXFBConfig config, int renderType,GLXContext shareList, Bool direct)
{
    _glXCreateNewContextFunc newFunc = glmdGetFuncAddr("glXCreateNewContext");
    glmdCreateContext();
    return newFunc(dpy, config, renderType, shareList, direct);
}

extern glxFuncPointer
glXGetProcAddress(const GLubyte* name)
{
    _glXGetProcAddressFunc newFunc = glmdGetFuncAddr("glXGetProcAddress");
    return newFunc(name);

}
